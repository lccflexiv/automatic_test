/**
 * @file ManageJson.hpp
 * @copyright Copyright (C) 2016-2022 Flexiv Ltd. All Rights Reserved.
 */

#ifndef FLEXIVRDK_MANAGEJSON_HPP_
#define FLEXIVRDK_MANAGEJSON_HPP_
// autotest headers
#include <autotest/SystemParams.h>
#include <autotest/RobotOperation.hpp>

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
        g_goalPlanList.push_back(item);
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
    for (it = g_goalPlanList.begin(); it != g_goalPlanList.end(); it++){
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

int checkJson(flexiv::Robot* robotPtr, std::string filePath, std::string jsonFileName, flexiv::Log* logPtr){
    int result;
    g_realPlanList = robotPtr->getPlanNameList();
    for (std::string eachPlan : g_goalPlanList)
    {
        if (std::find(g_realPlanList.begin(), g_realPlanList.end(), eachPlan) == g_realPlanList.end()){
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
#endif /* FLEXIVRDK_MANAGEJSON_HPP_ */