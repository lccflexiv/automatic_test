/*
 * @file RobotOperation.hpp
 * @copyright Copyright (C) 2016-2022 Flexiv Ltd. All Rights Reserved.
 */

#ifndef FLEXIVRDK_ROBOTOPERATIONS_HPP_
#define FLEXIVRDK_ROBOTOPERATIONS_HPP_
// autotest headers
#include <autotest/SystemParams.h>

// This function make robot execute the plan and wait for it to finish
int executeRobotPlan(flexiv::Robot* robot, std::string planName, flexiv::Log* logPtr){
    flexiv::SystemStatus systemStatus;
    robot->executePlanByName(planName);
    while (systemStatus.m_programRunning == false)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
        robot->getSystemStatus(&systemStatus);
    }
    while (systemStatus.m_programRunning == true)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
        robot->getSystemStatus(&systemStatus);
    }
    
    logPtr->info("Robot has executed plan: " + planName);
    return SUCCESS;
}
#endif /* FLEXIVRDK_ROBOTOPERATIONS_HPP_ */