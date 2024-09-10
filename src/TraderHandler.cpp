#include <iostream>
#include <thread>
#include <string>

#ifdef __linux__
#include <bsd/string.h>
#endif
#include "TraderHandler.h"
#include "utils.h"

#define Log(x) cout << x << endl;

using namespace std;

CTraderHandler::CTraderHandler(
    std::string EnvName,
    std::vector<std::string> TraderFrontAddr,
    std::string TraderConDir,
    std::string BrokerID,
    std::string UserID,
    std::string InvestorID,
    std::string Password,
    std::string AppID,
    std::string AuthCode,
    std::string LoginRemark
):
    EnvName(EnvName),
    TraderFrontAddr(TraderFrontAddr),
    TraderConDir(TraderConDir),
    BrokerID(BrokerID),
    UserID(UserID),
    InvestorID(InvestorID),
    Password(Password),
    AppID(AppID),
    AuthCode(AuthCode),
    LoginRemark(LoginRemark)
{
    std::cout << "【CTraderHandler】constructed using Env: " << EnvName << " with BrokerID: " << BrokerID << " UserID: " << UserID << " InvestorID: " << \
        InvestorID <<  " PasswordLen: " <<  Password.length() << " Writing Connection info to " << TraderConDir << std::endl;
}

CTraderHandler::~CTraderHandler()
{
    Log("【CTraderHandler】 Released");
    m_ptraderapi->Release();
}

void CTraderHandler::connect()
{
    Log("【CTraderHandler】Request Connection");
    m_ptraderapi = CThostFtdcTraderApi::CreateFtdcTraderApi(TraderConDir.c_str());
    m_ptraderapi->RegisterSpi(this);
    m_ptraderapi->SubscribePublicTopic(THOST_TERT_QUICK);
    m_ptraderapi->SubscribePrivateTopic(THOST_TERT_QUICK); // 设置私有流订阅模式
    for (const std::string& i : TraderFrontAddr)
    {
        Log(std::string("【CTraderHandler】trader target fron:") + i);
        m_ptraderapi->RegisterFront((char*)i.c_str());
    }

    m_ptraderapi->Init();
    Log(std::string("【CTraderHandler】Inited api version is: ") + std::string(m_ptraderapi->GetApiVersion()));
}

void CTraderHandler::awaitConnectionPromise()
{
    connectionPromise.get_future().get();
}

void CTraderHandler::release()
{
    Log("【CTraderHandler】 Released");
    m_ptraderapi->Release();
}

void CTraderHandler::authenticate()
{
    Log("【CTraderHandler】Request Authenticate start");
    int nRequestID = 0;
    CThostFtdcReqAuthenticateField a = {0};
    strlcpy(a.BrokerID, BrokerID.c_str(), BrokerID.size());
    strlcpy(a.UserID, UserID.c_str(), UserID.size());
    strlcpy(a.AuthCode, AuthCode.c_str(), AuthCode.size());
    strlcpy(a.AppID, AppID.c_str(), AppID.size());
    Log("【CTraderHandler】Request Authenticate end");
    m_ptraderapi->ReqAuthenticate(&a, nRequestID++);
}

void CTraderHandler::awaitAuthenticatePromise()
{
    authenticatePromise.get_future().get();
}

void CTraderHandler::login()
{
    Log("【CTraderHandler】Request Logining")
    int nRequestID = 0;
    CThostFtdcReqUserLoginField t = {0};
    strlcpy(t.BrokerID, BrokerID.c_str(), BrokerID.size() + 1);
    strlcpy(t.UserID, UserID.c_str(), UserID.size() + 1);
    strlcpy(t.Password, Password.c_str(), Password.size() + 1);
    strlcpy(t.LoginRemark, LoginRemark.c_str(), LoginRemark.size() + 1);
    while (1) {
        #ifdef __linux__
        int respCode = m_ptraderapi->ReqUserLogin(&t, nRequestID++);
        #else
        int respCode = m_ptraderapi->ReqUserLogin(&t, nRequestID++, 0, (char*)"");
        #endif

        Log("【CTraderHandler】Request login status: " + std::to_string(respCode));
        if (respCode == 0) {
            Log("【CTraderHandler】request login successfully");
            break;
        } else {
            Log("【CTraderHandler】request login Failed, retry after 5s");
            std::this_thread::sleep_for(5s);
        }
    };
}

void CTraderHandler::awaitLoginPromise()
{
    loginPromise.get_future().get();
}

void CTraderHandler::qrySettlementInfo()
{
    Log("【CTraderHandler】Request qrySettlementInfo");
    int nRequestID = 0;
    CThostFtdcQrySettlementInfoField a = {
        .TradingDay="20240903"
    };
    strlcpy(a.BrokerID, BrokerID.c_str(), BrokerID.size());
    strlcpy(a.InvestorID, InvestorID.c_str(), InvestorID.size());
    int respCpde = m_ptraderapi->ReqQrySettlementInfo(&a, nRequestID++);
    Log(std::string("【CTraderHandler】Finish request qrySettlementInfo") + std::to_string(respCpde));
}

void CTraderHandler::awaitSettlementInfoQryPromise()
{
    settlementInfoQryPromise.get_future().get();
}

void CTraderHandler::settlementInfoConfirm()
{
    Log("【CTraderHandler】Request settlementinfoConfirm");
    CThostFtdcSettlementInfoConfirmField t = {0};
    strlcpy(t.BrokerID, BrokerID.c_str(), BrokerID.size() + 1);
    strlcpy(t.InvestorID, InvestorID.c_str(), InvestorID.size() + 1);
    while (1) {
        int nRequestId = 0;
        int respCode = m_ptraderapi->ReqSettlementInfoConfirm(&t, nRequestId++);
        if (respCode == 0) {
            Log("【CTraderHandler】request settlementinfoConfirm successfully");
            break;
        } else {
            Log("【CTraderHandler】request settlementinfoConfirm failed, retry after 60s");
            std::this_thread::sleep_for(60s);
        }
    }
}

void CTraderHandler::awaitSettlementInfoConfirmPromise()
{
    settlementInfoConfirmPromise.get_future().get();
}

void CTraderHandler::orderinsert()
{
    CThostFtdcInputOrderField t = {
        .OrderRef = "666666", // 用户端id
        .OrderPriceType = THOST_FTDC_OPT_LimitPrice, // 限价单
        .Direction = THOST_FTDC_D_Buy, // 确定开平标志：开平方向
        .LimitPrice = 7090,
        .VolumeTotalOriginal = 1, // 数量
        .TimeCondition = THOST_FTDC_TC_GFD, // 有效期类型：当日有效
        .VolumeCondition = THOST_FTDC_VC_AV, // 确定成交量类型（任意）
        .ContingentCondition = THOST_FTDC_CC_Immediately, // 确定触发条件：立即
        .StopPrice = 3213,
        .ForceCloseReason = THOST_FTDC_FCC_NotForceClose, // 强平：普通用户下单填写THOST_FTDC_FCC_NotForceClose 非强平
        .IsAutoSuspend = 0,
        .IsSwapOrder = false,
        .ExchangeID = "SHFE",
        .InstrumentID = "ag2410", // 合约代码
    };

    strlcpy(t.BrokerID, BrokerID.c_str(), BrokerID.size());
    strlcpy(t.InvestorID, InvestorID.c_str(), InvestorID.size());
    t.CombOffsetFlag[0] = THOST_FTDC_OF_Open; // 确定开平标志：开平方向
    t.CombHedgeFlag[0] = THOST_FTDC_HF_Speculation; // 确定投机套保标志：投机
    strlcpy(t.UserID, UserID.c_str(), UserID.size());
    while (m_ptraderapi->ReqOrderInsert(&t, 3) != 0) std::this_thread::sleep_for(10s);
}

void CTraderHandler::qryInstrument()
{
    Log("【CTraderHandler】Request qryInstrument");
    CThostFtdcQryInstrumentField t = {0};
    while (1) {
        int nRequestId = 0;
        int respCode = m_ptraderapi->ReqQryInstrument(&t, nRequestId++);
        if (respCode == 0) {
            Log("【CTraderHandler】request qryInstrument successfully");
            break;
        } else {
            Log("【CTraderHandler】request qryInstrument failed, retry after 10s");
            std::this_thread::sleep_for(10s);
        }
    }
}

void CTraderHandler::OnFrontConnected()
{
    Log("【CTraderHandler】OnFrontConnected");
    std::call_once(connectionOnce, [&]() {connectionPromise.set_value(0);});
}

void CTraderHandler::OnRspAuthenticate(CThostFtdcRspAuthenticateField *pRspAuthenticateField, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
    CtpRspInfo a = extractCtpRespInfo(pRspInfo);
    Log("【CTraderHandler】OnRspAuthenticate status: " + std::to_string(a.status) + std::string(", error msg:") + std::string(a.message));
    std::call_once(authenticateOnce, [&]() {authenticatePromise.set_value(a.status);});
}

void CTraderHandler::OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
    CtpRspInfo a = extractCtpRespInfo(pRspInfo);
    Log("【CTraderHandler】OnRspUserLogin status: " + std::to_string(a.status) + std::string(", error msg:") + std::string(a.message));
    Log(std::string("【CTraderHandler】FrontID: ") + std::to_string(pRspUserLogin->FrontID)); // 客户端连接收到的前置机编号
    Log(std::string("【CTraderHandler】SessionID: ") + std::to_string(pRspUserLogin->SessionID)); // 客户端连接的会话编号
    Log(std::string("【CTraderHandler】MaxOrderRef: ") + std::string(pRspUserLogin->MaxOrderRef)); //每一笔保单都一个唯一的不重复的编号（OrderRef）若客户端不赋值，服务端自动赋值，向上递增防止重复
    std::call_once(loginOnce, [&]() {loginPromise.set_value(a.status);});
}


void CTraderHandler::OnRspQrySettlementInfo(CThostFtdcSettlementInfoField *pSettlementInfo, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
    CtpRspInfo a = extractCtpRespInfo(pRspInfo);
    Log("【CTraderHandler】OnRspQrySettlementInfo status: " + std::to_string(a.status) + std::string(", error msg:") + std::string(a.message));
    if (!a.isNullPtr) {
        Log(pSettlementInfo->Content);
    }

    if (bIsLast) {
        std::call_once(settlementInfoQryOnce, [&]() {settlementInfoQryPromise.set_value(a.status);});
    }
}

void CTraderHandler::OnRspSettlementInfoConfirm(CThostFtdcSettlementInfoConfirmField *pSettlementInfoConfirm, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
    Log("【CTraderHandler】OnRspSettlementInfoConfirm status: " + std::to_string(pRspInfo->ErrorID) + std::string(", error msg:") + std::string(pRspInfo->ErrorMsg));
    std::call_once(settlementInfoConfirmOnce, [&]() {settlementInfoConfirmPromise.set_value(0);});
}

void CTraderHandler::OnRspQryInstrument(CThostFtdcInstrumentField *pInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
    auto end = std::chrono::system_clock::now();
    std::time_t end_time = std::chrono::system_clock::to_time_t(end);
    auto d = std::ctime(&end_time);
    Log("【CTraderHandler】OnRspQryInstrument" + std::to_string(nRequestID) + std::to_string(bIsLast) + std::string(d));
    instructmentStore[pInstrument->InstrumentID] = *pInstrument;
}

void CTraderHandler::OnRtnOrder(CThostFtdcOrderField *pOrder)
{
    Log("【CTraderHandler】OnRtnOrder");
    Log(pOrder->OrderStatus);
    Log(pOrder->StatusMsg);
    Log("【CTraderHandler】onRtnOrder Finished");
}

void CTraderHandler::OnRtnTrade(CThostFtdcTradeField *pTrade)
{
    Log("【CTraderHandler】OnRtnTrade");
}

void CTraderHandler::OnRspOrderInsert(CThostFtdcInputOrderField *pInputOrder, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
    Log("【CTraderHandler】OnRspOrderInsert");
    Log(pRspInfo -> ErrorID);
}

void CTraderHandler::OnRspError(CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
    Log("【CTraderHandler】OnRspError");
}
