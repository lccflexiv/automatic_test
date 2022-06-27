/**
 * @file ManageList.hpp
 * @copyright Copyright (C) 2016-2022 Flexiv Ltd. All Rights Reserved.
 */

#ifndef FLEXIVRDK_MANAGELIST_HPP_
#define FLEXIVRDK_MANAGELIST_HPP_
// autotest headers
#include <autotest/SystemParams.h>
#include <autotest/RobotOperation.hpp>

int generateCSV(flexiv::Robot* robotPtr, std::string filePath, std::string csvFileName, flexiv::Log* logPtr)
{
    std::string suffixStr = csvFileName.substr(csvFileName.find_last_of('.') + 1);
    if (suffixStr != "csv")
    {
        logPtr->error("The suffix of the input file name is not csv!");
        return CSV;
    }
    std::ofstream outputCSVFile(filePath+csvFileName, std::ios::out);
    if(outputCSVFile.fail())
    {
        logPtr->error("Failed to generate CSV file: " + csvFileName);
        return CSV;
    }
    g_realPlanList = robotPtr->getPlanNameList();
    for (std::string eachPlan : g_realPlanList)
    {
        outputCSVFile<<eachPlan<<std::endl;
    }
    outputCSVFile.close();
    return SUCCESS;
}

int readCSV(std::string filePath, std::string csvFileName, flexiv::Log* logPtr)
{
    std::string suffixStr = csvFileName.substr(csvFileName.find_last_of('.') + 1);
    if (suffixStr != "csv")
    {
        logPtr->error("The suffix of the input file name is not csv!");
        return CSV;
    }
    std::ifstream inputCSVFile(filePath+csvFileName, std::ios::in);
    if(inputCSVFile.fail())
    {
        logPtr->error("Failed to read file: " + csvFileName);
        return CSV;
    }
    std::string item;
    while(getline(inputCSVFile, item))
    {
        if (item[0] == '$'){
            g_goalPlanList.push_back(item.substr(item.find_first_of('$')+1));
        }
    }
    inputCSVFile.close();
    return SUCCESS;
}
#endif /* FLEXIVRDK_MANAGELIST_HPP_ */