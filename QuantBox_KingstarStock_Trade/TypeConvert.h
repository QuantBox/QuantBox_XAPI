#pragma once

#include "../include/CTP/ThostFtdcUserApiDataType.h"
#include "../include/CTP/ThostFtdcUserApiStruct.h"
#include "../include/ApiStruct.h"


PutCall TThostFtdcOptionsTypeType_2_PutCall(TThostFtdcOptionsTypeType In);

HedgeFlagType TThostFtdcHedgeFlagType_2_HedgeFlagType(TThostFtdcHedgeFlagType In);
TThostFtdcHedgeFlagType HedgeFlagType_2_TThostFtdcHedgeFlagType(HedgeFlagType In);

TThostFtdcOffsetFlagType OpenCloseType_2_TThostFtdcOffsetFlagType(OpenCloseType In);
OpenCloseType TThostFtdcOffsetFlagType_2_OpenCloseType(TThostFtdcOffsetFlagType In);

TThostFtdcDirectionType OrderSide_2_TThostFtdcDirectionType(OrderSide In);
OrderSide TThostFtdcDirectionType_2_OrderSide(TThostFtdcDirectionType In);
PositionSide TThostFtdcPosiDirectionType_2_PositionSide(TThostFtdcPosiDirectionType In);
PositionSide TradeField_2_PositionSide(TradeField* pIn);

TThostFtdcOrderPriceTypeType OrderType_2_TThostFtdcOrderPriceTypeType(OrderType In);

OrderType CThostFtdcOrderField_2_OrderType(CThostFtdcOrderField* pIn);
TimeInForce CThostFtdcOrderField_2_TimeInForce(CThostFtdcOrderField* pIn);

OrderStatus CThostFtdcOrderField_2_OrderStatus(CThostFtdcOrderField* pIn);
ExecType CThostFtdcOrderField_2_ExecType(CThostFtdcOrderField* pIn);

OrderStatus CThostFtdcQuoteField_2_OrderStatus(CThostFtdcQuoteField* pIn);
ExecType CThostFtdcQuoteField_2_ExecType(CThostFtdcQuoteField* pIn);

InstrumentType CThostFtdcInstrumentField_2_InstrumentType(CThostFtdcInstrumentField* pIn);
