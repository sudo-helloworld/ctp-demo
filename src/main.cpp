#include <iostream>
#include <future>
#include <thread>

#include "MdHandler.h"
#include "TraderHandler.h"
#include "config.h"

#define Log(x) cout << x << endl;

using namespace std;
using namespace std::literals::chrono_literals;

int main()
{
    Log("Hello World!");
    SystemConfig config = load_config();
    /** 行情部分demo */
    CMduserHandler *mduser = new CMduserHandler(
        config.MdFrontAddr,
        config.MdConDir,
        config.BrokerID,
        config.UserID,
        config.Password,
        config.LoginRemark
    );
    mduser->connect();
    mduser->awaitConnectionPromise();
    mduser->login();
    mduser->awaitLoginPromise();
    mduser->subscribe();
    mduser->awaitSubscribePromise();

    /** 交易部分demo */
    CTraderHandler *trader = new CTraderHandler(
        config.EnvName,
        config.TraderFrontAddr,
        config.TraderConDir,
        config.BrokerID,
        config.UserID,
        config.InvestorID,
        config.Password,
        config.AppID,
        config.AuthCode,
        config.LoginRemark
    );
    trader->connect();
    trader->awaitConnectionPromise();
    trader->authenticate();
    trader->awaitAuthenticatePromise();
    trader->login();
    trader->awaitLoginPromise();
    trader->qrySettlementInfo();
    trader->awaitSettlementInfoQryPromise();
    trader->settlementInfoConfirm();
    trader->awaitSettlementInfoConfirmPromise();
    trader->qryInstrument();
    // std::this_thread::sleep_for(5s);
    // trader->orderinsert();

    while(1) {
        Log("Heartbeat");
        std::this_thread::sleep_for(120s);
    }

    std::promise<void>().get_future().wait(); // sleep forever
    return 0;
}
