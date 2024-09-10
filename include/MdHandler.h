#pragma once

#include <vector>
#include <string>
#include <future>

#include "ThostFtdcMdApi.h"

enum CMdDisconnectReason {
    ///        0x1001 网络读失败
    ///        0x1002 网络写失败
    ///        0x2001 接收心跳超时
    ///        0x2002 发送心跳失败
    ///        0x2003 收到错误报文
    CMD_DISCONNECT_FAIL_READ = 0x1001,
    CMD_DISCONNECT_FAIL_WRITE = 0x1002,
    CMD_DISCONNECT_TIMEOUT_HEARTBEAT = 0x2001,
    CMD_DISCONNECT_FAIL_SEND_HEARTBEAT = 0x2002,
    CMD_DISCONNECT_WRONG_RESP = 0x2003
};

class CMduserHandler: public CThostFtdcMdSpi
{
private:
    CThostFtdcMdApi* m_mdApi;

    std::vector<std::string> MdFrontAddr;
    std::string MdConDir;
    std::string BrokerID;
    std::string UserID;
    std::string Password;
    std::string LoginRemark;

    std::once_flag connectionOnce;
    std::promise<int> connectionPromise;
    std::once_flag loginOnce;
    std::promise<int> loginPromise;
    std::once_flag subscribeOnce;
    std::promise<int> subscribePromise;

public:
    CMduserHandler(
        std::vector<std::string> MdFrontAddr,
        std::string MdConDir,
        std::string BrokerID,
        std::string UserID,
        std::string Password,
        std::string LoginRemark
    );

    void connect();
    void login();
    void subscribe();

    int awaitConnectionPromise(); // promise前置机连接请求
    int awaitLoginPromise(); // promise登录请求
    int awaitSubscribePromise(); // promise订阅请求

    void OnFrontConnected();
    void OnFrontDisconnected(int nReason);
    void OnHeartBeatWarning(int nTimeLapse);
    void OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
    void OnRspSubMarketData(CThostFtdcSpecificInstrumentField *pSpecificInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
    void OnRtnDepthMarketData(CThostFtdcDepthMarketDataField *pDepthMarketData);
};
