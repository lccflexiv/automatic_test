/**
 * @file SystemParams.h
 * @copyright Copyright (C) 2016-2022 Flexiv Ltd. All Rights Reserved.
 */

#ifndef FLEXIVRDK_SYSTEMPARAMS_HPP_
#define FLEXIVRDK_SYSTEMPARAMS_HPP_
//Flexiv headers
#include <flexiv/Robot.hpp>
#include <flexiv/Exception.hpp>
#include <flexiv/Log.hpp>

//Regular headers
#include <jsoncpp/json/json.h>
#include <string>
#include <iostream>
#include <fstream>
#include <cmath>
#include <mutex>
#include <thread>
#include <chrono>
#include <algorithm>
std::vector<std::string> g_goalPlanList;
std::vector<std::string> g_realPlanList;
enum Status{SUCCESS, ROBOT, CSV, JSON, UNKNOWN};
std::string robotIP = "127.0.0.1"; // IP of the robot server
std::string localIP = "127.0.0.1"; // IP of the workstation PC running this program
#endif /* FLEXIVRDK_SYSTEMPARAMS_HPP_ */