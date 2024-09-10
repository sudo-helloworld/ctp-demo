#include <iostream>

#include "utils.h"

CtpRspInfo extractCtpRespInfo(CThostFtdcRspInfoField *pRspInfo) // 封装一下避免取ErrorID的时候空指针错误
{
    bool isNullPtr = pRspInfo == nullptr;
    int status = isNullPtr ? 0 : pRspInfo->ErrorID;
    const char* message = isNullPtr ? "NULL_PTR" : pRspInfo->ErrorMsg;
    if (isNullPtr)
    {
        std::cout << "NULL PTR FOR *pRspInfo" << std::endl;
    }

    return {
        status,
        message,
        isNullPtr
    };
}
