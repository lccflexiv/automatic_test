/**
 * @example Run auto-recovery if the robot's safety system is in recovery state,
 * otherwise run damped floating with joint APF disabled, so that the user can
 * manually trigger a safety system recovery state by moving any joint close to
 * its limit. See flexiv::Mode::MODE_AUTO_RECOVERY for more details.
 * @copyright (C) 2016-2021 Flexiv Ltd. All Rights Reserved.
 * @author Flexiv
 */

#include <config.h>
#include <Robot.hpp>

#include <iostream>
#include <string>
#include <thread>

namespace {
// robot DOF
const int k_robotDofs = 7;

const std::vector<double> k_floatingDamping
    = {10.0, 10.0, 5.0, 5.0, 1.0, 1.0, 1.0};

}

// callback function for realtime periodic task
void periodicTask(std::shared_ptr<flexiv::RobotStates> robotStates,
    std::shared_ptr<flexiv::Robot> robot)
{
    // read robot states
    robot->getRobotStates(robotStates.get());

    // set 0 joint torques
    std::vector<double> targetTorque(k_robotDofs, 0.0);

    // add some velocity damping
    for (size_t i = 0; i < k_robotDofs; ++i) {
        targetTorque[i]
            = -k_floatingDamping[i] * robotStates->m_linkVelocity[i];
    }

    // send target joint torque to RDK server, enable gravity compensation
    // and joint-space APF
    robot->streamJointTorque(targetTorque, true, false);
}

int main(int argc, char* argv[])
{
    // print loop frequency
    std::cout << "Example client running at 1000 Hz" << std::endl;

    // RDK Initialization
    //=============================================================================
    // RDK robot interface
    auto robot = std::make_shared<flexiv::Robot>();

    // robot states data from RDK server
    auto robotStates = std::make_shared<flexiv::RobotStates>();

    // initialize connection
    robot->init(ROBOT_IP, LOCAL_IP);

    // wait for the connection to be established
    do {
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    } while (!robot->isConnected());

    // enable the robot, make sure the E-stop is released before enabling
    if (robot->enable()) {
        std::cout << "Enabling robot ..." << std::endl;
    }

    // if the system is in recovery state, we can't use isOperational to tell if
    // the enable process is done, so just wait for long enough
    std::this_thread::sleep_for(std::chrono::seconds(8));

    // Auto recover if needed
    //=============================================================================
    // start auto recovery if the system is in recovery state, the involved
    // joints will start to move back into allowed position range
    if (robot->isRecoveryState()) {
        robot->startAutoRecovery();
        // block forever, must reboot robot and restart user program after auto
        // recovery is done
        do {
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        } while (true);
    }
    // otherwise the system is normal, run robot floating using joint torques
    else {
        robot->setMode(flexiv::MODE_JOINT_TORQUE);
        // wait for the mode to be switched
        do {
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        } while (robot->getMode() != flexiv::MODE_JOINT_TORQUE);
    }

    // High-priority Realtime Periodic Task @ 1kHz
    //=============================================================================
    // this is a blocking method, so all other user-defined background
    // threads should be spawned before this
    robot->start(std::bind(periodicTask, robotStates, robot));

    return 0;
}