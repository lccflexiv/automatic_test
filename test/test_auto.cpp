/**
 * @example test_auto.cpp
 * Execute the plan list by using RDK's plan execution API.
 * @copyright Copyright (C) 2016-2021 Flexiv Ltd. All Rights Reserved.
 * @author Flexiv
 */
//flexiv headers
#include <flexiv/Robot.hpp>
#include <flexiv/Exception.hpp>
#include <flexiv/Log.hpp>
// autotest headers
#include <autotest/SystemParams.h>
#include <autotest/RobotOperation.hpp>
#include <autotest/ManageJson.hpp>

int main()
{
    // Log object for printing message with timestamp and coloring
    flexiv::Log log;
    int result;
    try {
        // RDK Initialization
        flexiv::Robot robot(robotIP, localIP);

        // Clear fault on robot server if any
        if (robot.isFault()) {
            log.warn("Fault occurred on robot server, trying to clear ...");
            // Try to clear the fault
            robot.clearFault();
            std::this_thread::sleep_for(std::chrono::seconds(2));
            // Check again
            if (robot.isFault()) {
                log.error("Fault cannot be cleared, exiting ...");
                return ROBOT;
            }else
            log.info("Fault on robot server is cleared");
        }

        // Enable the robot, make sure the E-stop is released before enabling
        log.info("Enabling robot ...");
        try {
            //robot.enable();
        } catch (const flexiv::Exception& e) {
            log.error(e.what());
            return ROBOT;
        }

        // Wait for the robot to become operational
        while (!robot.isOperational()) {
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
        log.info("Robot is now operational");

        // Set mode after robot is operational
        robot.setMode(flexiv::MODE_PLAN_EXECUTION);

        // Wait for the mode to be switched
        while (robot.getMode() != flexiv::MODE_PLAN_EXECUTION) {
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
        log.info("Robot is now in plan execution mode");
        log.info("---------------start executing plans in csv list---------------");
        
        //Import csv files and Json files
        std::string filePath = "/home/ae/flexiv_rdk_versions/flexiv_rdk_autotest/test/";
        std::string csvFileName = "list.csv";
        std::string jsonFileName = "list1.json";
        result = generateJSON(filePath, csvFileName, jsonFileName, &log);
        result = checkJson(&robot, filePath, jsonFileName, &log);
        result = readJSON(&robot, filePath, jsonFileName, &log);
        robot.setMode(flexiv::MODE_IDLE);
    } catch (const flexiv::Exception& e) {
        log.error(e.what());
        return UNKNOWN;
    }

    return 0;
}
