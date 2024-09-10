#pragma once

#include <string>
#include <vector>

struct SystemConfig
{
    std::string EnvName;
    std::vector<std::string> MdFrontAddr;
    std::vector<std::string> TraderFrontAddr;
    std::string BrokerID;
    std::string UserID;
    std::string InvestorID;
    std::string Password;
    std::string AppID;
    std::string AuthCode;
    std::string LoginRemark;
    std::string SystemInfoRemark;
    std::string MdConDir;
    std::string TraderConDir;
};

SystemConfig load_config();
