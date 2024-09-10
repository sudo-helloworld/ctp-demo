#include <thread>
#include <iostream>
#include <string>

#ifdef __linux__
#include <bsd/string.h>
#endif
#include "MdHandler.h"
#include "utils.h"

#define Log(x) cout << x << std::endl;

using namespace std;
using namespace std::literals::chrono_literals;

CMduserHandler::CMduserHandler(
    std::vector<std::string> MdFrontAddr,
    std::string MdConDir,
    std::string BrokerID,
    std::string UserID,
    std::string Password,
    std::string LoginRemark
):
    MdFrontAddr(MdFrontAddr),
    MdConDir(MdConDir),
    BrokerID(BrokerID),
    UserID(UserID),
    Password(Password),
    LoginRemark(LoginRemark)
{
    std::cout << "【CMduserHandler】Constructed with BrokerID: " << BrokerID << " UserID: " << UserID << " PasswordLen: " \
              <<  Password.length() << " Writing Connection info to " << MdConDir << std::endl;
}

void CMduserHandler::connect()
{
    Log("【CMduserHandler】Start connecting");
    m_mdApi = CThostFtdcMdApi::CreateFtdcMdApi(MdConDir.c_str(), true, true);
    m_mdApi->RegisterSpi(this);
    for (const std::string& i : MdFrontAddr)
    {
        Log(std::string("【CMduserHandler】mduser target front:") + i);
        m_mdApi->RegisterFront((char*)i.c_str());
    }
    m_mdApi->Init();
    Log(std::string("【CMduserHandler】Inited api version is: ") + std::string(m_mdApi->GetApiVersion()));
}

void CMduserHandler::login()
{
    Log("【CMduserHandler】Request logining")
    int nRequestID = 0;
    CThostFtdcReqUserLoginField t = { 0 };
    strlcpy(t.BrokerID, BrokerID.c_str(), BrokerID.size());
    strlcpy(t.UserID, UserID.c_str(), UserID.size());
    strlcpy(t.Password, Password.c_str(), Password.size());
    strlcpy(t.LoginRemark, LoginRemark.c_str(), LoginRemark.size());
    while (1) {
        int respCode = m_mdApi->ReqUserLogin(&t, nRequestID++);
        Log("【CMduserHandler】Request login status: " + std::to_string(respCode));
        if (respCode == 0) {
            Log("【CMduserHandler】request login successfully");
            break;
        } else {
            Log("【CMduserHandler】request login Failed, retry after 5s");
            std::this_thread::sleep_for(5s);
        }
    };
}

void CMduserHandler::subscribe()
{
    Log("【CMduserHandler】Request subscribing");
    int nRequestID = 0;
    char **ppInstrument=new char * [50];
    ppInstrument[0] = (char*)"IF2410";
    ppInstrument[1] = (char*)"IF2412";
    while (1) {
        int respCode = m_mdApi->SubscribeMarketData(ppInstrument, nRequestID++);
        Log("【CMduserHandler】Request subscribing status: " + std::to_string(respCode));
        if (respCode == 0) {
            Log("【CMduserHandler】request subscribing successfully");
            break;
        } else {
            Log("【CMduserHandler】request subscribing failed, retry after 5s");
            std::this_thread::sleep_for(5s);
        }
    };
}

int CMduserHandler::awaitConnectionPromise()
{
    return connectionPromise.get_future().get();
}

int CMduserHandler::awaitLoginPromise()
{
    return loginPromise.get_future().get();
}

int CMduserHandler::awaitSubscribePromise()
{
    return subscribePromise.get_future().get();
}

void CMduserHandler::OnFrontConnected()
{

    Log("【CMduserHandler】OnFrontConnected");
    std::call_once(connectionOnce, [&]() {connectionPromise.set_value(0);});
}

void CMduserHandler::OnFrontDisconnected(int nReason)
{
    // 当客户端与交易后台通信连接断开时，该方法被调用。当发生这个情况后，API会自动重新连接，客户端可不做处理。
    CMdDisconnectReason reason = static_cast<CMdDisconnectReason>(nReason);
    switch (reason) {
    case CMD_DISCONNECT_FAIL_READ:
        Log("【CMduserHandler】FrontDisconnected NETWORK READING FAILED");
        break;
    case CMD_DISCONNECT_FAIL_WRITE:
        Log("【CMduserHandler】FrontDisconnected NETWORK WRITING FAILED");
        break;
    case CMD_DISCONNECT_TIMEOUT_HEARTBEAT:
        Log("【CMduserHandler】FrontDisconnected HEARTBEAT TIMEOUT");
        break;
    case CMD_DISCONNECT_FAIL_SEND_HEARTBEAT:
        Log("【CMduserHandler】FrontDisconnected HEARTBEAT SEND FAILED");
        break;
    case CMD_DISCONNECT_WRONG_RESP:
        Log("【CMduserHandler】FrontDisconnected WRONG RESP");
        break;
    default:
        Log("【CMduserHandler】FrontDisconnected UNKNOW ERROR: " + std::to_string(reason));
        break;
    }
}

void CMduserHandler::OnHeartBeatWarning(int nTimeLapse)
{
    cout << "【CMduserHandler】OnHeartBeatWarning:  " << nTimeLapse << "s not received heartbeat\n";
};

void CMduserHandler::OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
    CtpRspInfo a = extractCtpRespInfo(pRspInfo);
    Log("【CMduserHandler】OnRspUserLogin status: " + std::to_string(a.status) + std::string(", error msg:") + std::string(a.message));
    Log(std::string("【CMduserHandler】FrontID: ") + std::to_string(pRspUserLogin->FrontID)); // 客户端连接收到的前置机编号
    Log(std::string("【CMduserHandler】SessionID: ") + std::to_string(pRspUserLogin->SessionID)); // 客户端连接的会话编号
    Log(std::string("【CMduserHandler】MaxOrderRef: ") + std::string(pRspUserLogin->MaxOrderRef)); //每一笔保单都一个唯一的不重复的编号（OrderRef）若客户端不赋值，服务端自动赋值，向上递增防止重复
    std::call_once(loginOnce, [&]() {loginPromise.set_value(0);});
};

void CMduserHandler::OnRtnDepthMarketData(CThostFtdcDepthMarketDataField *pDepthMarketData)
{
    std::cout << "【CMduserHandler】OnRtnDepthMarketData" << pDepthMarketData->InstrumentID << ": " << pDepthMarketData->LastPrice << "; " << pDepthMarketData->UpdateTime << "_" << pDepthMarketData->UpdateMillisec <<std::endl;
}

void CMduserHandler::OnRspSubMarketData(CThostFtdcSpecificInstrumentField *pSpecificInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
    CtpRspInfo a = extractCtpRespInfo(pRspInfo);
    std::call_once(subscribeOnce, [&]() {subscribePromise.set_value(a.status);});
    std::cout << "【CMduserHandler】OnRspSubMarketData" << nRequestID << bIsLast << std::endl;
}
