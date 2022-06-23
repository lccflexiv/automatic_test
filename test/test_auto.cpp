/**
 * @example test_auto.cpp
 * Execute the plan list by using RDK's plan execution API.
 * @copyright Copyright (C) 2016-2021 Flexiv Ltd. All Rights Reserved.
 * @author Flexiv
 */

#include <flexiv/Robot.hpp>
#include <flexiv/Exception.hpp>
#include <flexiv/Log.hpp>
#include <jsoncpp/json/json.h>
#include <string>
#include <iostream>
#include <fstream>
#include <cmath>
#include <mutex>
#include <thread>
#include <algorithm>
std::vector<std::string> goalPlanList;
std::vector<std::string> realPlanList;
enum Status{SUCCESS, ROBOT, CSV, JSON};

int readCSV(std::string filename, flexiv::Log* logPtr)
{
    std::ifstream csvFile(filename, std::ios::in);
    if(csvFile.fail())
    {
        logPtr->error("Failed to read file: " + filename);
        return CSV;
    }
    std::string item;
    while(getline(csvFile, item)){
        goalPlanList.push_back(item);
    }
    return SUCCESS;
}

int readJSON(flexiv::Robot* robotPtr, std::string filePath, std::string jsonFileName, flexiv::Log* logPtr){
    std::ifstream ijsonFile(filePath+jsonFileName);
    if(ijsonFile.fail())
    {
        logPtr->error("Failed to read JSON file: " + jsonFileName);
        return JSON;
    }
    Json::Value plan;
    Json::Reader reader;
    bool parseResult = reader.parse(ijsonFile, plan);
    ijsonFile.close();
    if (!parseResult){
        logPtr->error("Failed to parse JSON file: " + jsonFileName);
        return JSON;
    }
    Json::StyledWriter sw;
    std::string answer = sw.write(plan);
    std::cout<<answer<<std::endl;
    return SUCCESS;
}

int generateJSON(std::string filePath, std::string csvFileName, std::string jsonFileName, flexiv::Log* logPtr){
    int result = readCSV(filePath+csvFileName, logPtr);
    if (result!=SUCCESS){
        return result;
    }
    std::fstream jsonFile(filePath+jsonFileName, std::ios::out);
    if (!jsonFile.is_open()) {
        logPtr->error("Failed to create JSON file: " + jsonFileName);
        return JSON;
    }
    Json::Value plan;
    std::vector<std::string>::iterator it;
    for (it = goalPlanList.begin(); it != goalPlanList.end(); it++){
        plan[(*it)]["existance"] = "";
        plan[(*it)]["executed"] = "";
    }
    Json::StyledWriter sw;
    std::string answer = sw.write(plan);
    jsonFile.write(answer.c_str(),answer.size());
    jsonFile.close();
    return SUCCESS;
}
int modifyJSON(std::string filePath, 
               std::string jsonFileName, 
               std::string planName, 
               std::string existance, 
               std::string executed,
               flexiv::Log* logPtr)
{
    std::ifstream ijsonFile(filePath+jsonFileName);
    if(ijsonFile.fail())
    {
        logPtr->error("Failed to read JSON file: " + jsonFileName);
        return JSON;
    }
    Json::Value plan;
    Json::Reader reader;
    bool parseResult = reader.parse(ijsonFile, plan);
    ijsonFile.close();
    if (!parseResult){
        logPtr->error("Failed to parse JSON file: " + jsonFileName);
        return JSON;
    }
    plan[planName]["existance"]=existance;
    plan[planName]["executed"]=executed;
    Json::StyledWriter sw;
    std::string answer = sw.write(plan);

    std::ofstream ojsonFile(filePath+jsonFileName);
    if(ojsonFile.fail())
    {
        logPtr->error("Failed to modify JSON file: " + jsonFileName);
        return JSON;
    }
    ojsonFile.write(answer.c_str(),answer.size());
    ojsonFile.close();
    return SUCCESS;
}

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

int checkJson(flexiv::Robot* robotPtr, std::string filePath, std::string jsonFileName, flexiv::Log* logPtr){
    int result;
    realPlanList = robotPtr->getPlanNameList();
    for (std::string eachPlan : goalPlanList)
    {
        if (std::find(realPlanList.begin(), realPlanList.end(), eachPlan) == realPlanList.end()){
            logPtr->warn("Plan: "+eachPlan+" does not exist");
            result = modifyJSON(filePath, jsonFileName, eachPlan, "No", "No", logPtr);
        }   
        else{
            //execute the existed plan
            logPtr->info("Plan: "+eachPlan+" does exist");
            result = modifyJSON(filePath, jsonFileName, eachPlan, "Yes", "No", logPtr);
            result = executeRobotPlan(robotPtr, eachPlan, logPtr);
            if (result==SUCCESS){
                result = modifyJSON(filePath, jsonFileName, eachPlan, "Yes", "Yes", logPtr);
            }
        } 
    } 
    return result;
}

int main()
{
    // Log object for printing message with timestamp and coloring
    flexiv::Log log;

    // IP of the robot server
    std::string robotIP = "127.0.0.1";

    // IP of the workstation PC running this program
    std::string localIP = "127.0.0.1";
    int result;
    try {
        // RDK Initialization
        //=============================================================================
        // Instantiate robot interface
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
                return 0;
            }else
            log.info("Fault on robot server is cleared");
        }

        // Enable the robot, make sure the E-stop is released before enabling
        log.info("Enabling robot ...");
        try {
            //robot.enable();
        } catch (const flexiv::Exception& e) {
            log.error(e.what());
            return 0;
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
        //create associate json file name
        std::string filePath = "/home/ae/flexiv_rdk_versions/flexiv_rdk_autotest/test/";
        std::string csvFileName = "list.csv";
        std::string jsonFileName = "list1.json";
        result = generateJSON(filePath, csvFileName, jsonFileName, &log);
        result = checkJson(&robot, filePath, jsonFileName, &log);
        result = readJSON(&robot, filePath, jsonFileName, &log);
        robot.setMode(flexiv::MODE_IDLE);
    } catch (const flexiv::Exception& e) {
        log.error(e.what());
        return 0;
    }

    return 0;
}
