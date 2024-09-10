#pragma once
#include <string>
#include <future>
#include <vector>
#include <map>
#include "ThostFtdcTraderApi.h"
#include "ThostFtdcUserApiStruct.h"

class CTraderHandler: public CThostFtdcTraderSpi
{
private:
    CThostFtdcTraderApi *m_ptraderapi;
    std::map<std::string, CThostFtdcInstrumentField> instructmentStore;
    std::string EnvName;
    std::vector<std::string> TraderFrontAddr;
    std::string TraderConDir;
    std::string BrokerID;
    std::string UserID;
    std::string InvestorID;
    std::string Password;
    std::string AppID;
    std::string AuthCode;
    std::string LoginRemark;

    std::once_flag connectionOnce;
    std::promise<int> connectionPromise;
    std::once_flag authenticateOnce;
    std::promise<int> authenticatePromise;
    std::once_flag loginOnce;
    std::promise<int> loginPromise;
    std::once_flag settlementInfoQryOnce;
    std::promise<int> settlementInfoQryPromise;
    std::once_flag settlementInfoConfirmOnce;
    std::promise<int> settlementInfoConfirmPromise;

public:
    CTraderHandler(
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
    );
    ~CTraderHandler();

    void connect();
    void release();
    void authenticate();
    void login();
    void settlementInfoConfirm();
    void qryInstrument();
    void qrySettlementInfo();
    void orderinsert();

    void awaitConnectionPromise(); // promise 前置机连接请求
    void awaitAuthenticatePromise(); // promise 客户端认证的请求
    void awaitLoginPromise(); // promise 登陆请求
    void awaitSettlementInfoQryPromise(); // promise 结算单查询请求
    void awaitSettlementInfoConfirmPromise(); // promise 结算单确认请求

    void OnFrontConnected();
    void OnRspAuthenticate(CThostFtdcRspAuthenticateField *pRspAuthenticateField, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
    void OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
    void OnRspQrySettlementInfo(CThostFtdcSettlementInfoField *pSettlementInfo, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
    void OnRspSettlementInfoConfirm(CThostFtdcSettlementInfoConfirmField *pSettlementInfoConfirm, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
    void OnRspQryInstrument(CThostFtdcInstrumentField *pInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
    void OnRtnOrder(CThostFtdcOrderField *pOrder);
    void OnRtnTrade(CThostFtdcTradeField *pTrade);
    void OnRspOrderInsert(CThostFtdcInputOrderField *pInputOrder, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
    void OnRspError(CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
};
