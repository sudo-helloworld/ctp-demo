#pragma once

#include "ThostFtdcMdApi.h"

struct CtpRspInfo {
    int status; // 状态码
    const char* message; // 错误消息
    bool isNullPtr;
};

CtpRspInfo extractCtpRespInfo(CThostFtdcRspInfoField *pRspInfo);
