#include <fstream>

#include "config.h"
#include "json.hpp"

using namespace std;
using json = nlohmann::json;

SystemConfig load_config()
{
    std::ifstream f("account-config.json");
    json data = json::parse(f);
    return {
        .EnvName=data["EnvName"],
        .MdFrontAddr=data["MdFrontAddr"],
        .TraderFrontAddr=data["TraderFrontAddr"],
        .BrokerID=data["BrokerID"],
        .UserID=data["UserID"],
        .InvestorID=data["InvestorID"],
        .Password=data["Password"],
        .AppID=data["AppID"],
        .AuthCode=data["AuthCode"],
        .LoginRemark=data["LoginRemark"],
        .SystemInfoRemark=data["SystemInfoRemark"],
        .MdConDir=data["MdConDir"],
        .TraderConDir=data["TraderConDir"]
    };
}
