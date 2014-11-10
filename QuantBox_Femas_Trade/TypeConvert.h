#pragma once

#include "../include/Femas/USTPFtdcUserApiDataType.h"
#include "../include/Femas/USTPFtdcUserApiStruct.h"
#include "../include/ApiStruct.h"


PutCall TUstpFtdcOptionsTypeType_2_PutCall(TUstpFtdcOptionsTypeType In);

HedgeFlagType TUstpFtdcHedgeFlagType_2_HedgeFlagType(TUstpFtdcHedgeFlagType In);
TUstpFtdcHedgeFlagType HedgeFlagType_2_TUstpFtdcHedgeFlagType(HedgeFlagType In);

TUstpFtdcOffsetFlagType OpenCloseType_2_TUstpFtdcOffsetFlagType(OpenCloseType In);
OpenCloseType TUstpFtdcOffsetFlagType_2_OpenCloseType(TUstpFtdcOffsetFlagType In);

TUstpFtdcDirectionType OrderSide_2_TUstpFtdcDirectionType(OrderSide In);
OrderSide TUstpFtdcDirectionType_2_OrderSide(TUstpFtdcDirectionType In);
PositionSide TUstpFtdcDirectionType_2_PositionSide(TUstpFtdcDirectionType In);

TUstpFtdcOrderPriceTypeType OrderType_2_TUstpFtdcOrderPriceTypeType(OrderType In);

OrderType CUstpFtdcOrderField_2_OrderType(CUstpFtdcOrderField* pIn);
TimeInForce CUstpFtdcOrderField_2_TimeInForce(CUstpFtdcOrderField* pIn);

OrderStatus CUstpFtdcOrderField_2_OrderStatus(CUstpFtdcOrderField* pIn);
ExecType CUstpFtdcOrderField_2_ExecType(CUstpFtdcOrderField* pIn);

OrderStatus CUstpFtdcRtnQuoteField_2_OrderStatus(CUstpFtdcRtnQuoteField* pIn);
ExecType CUstpFtdcRtnQuoteField_2_ExecType(CUstpFtdcRtnQuoteField* pIn);

InstrumentType CUstpFtdcRspInstrumentField_2_InstrumentType(CUstpFtdcRspInstrumentField* pIn);
