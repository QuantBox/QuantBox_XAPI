#pragma once

#include "../include/Kingstar_Stock/SPXAPI_struct.h"
#include "../include/ApiStruct.h"
#include <stdlib.h>

//PutCall TThostFtdcOptionsTypeType_2_PutCall(TThostFtdcOptionsTypeType In);

//HedgeFlagType TThostFtdcHedgeFlagType_2_HedgeFlagType(TThostFtdcHedgeFlagType In);
//TThostFtdcHedgeFlagType HedgeFlagType_2_TThostFtdcHedgeFlagType(HedgeFlagType In);
//
//TThostFtdcOffsetFlagType OpenCloseType_2_TThostFtdcOffsetFlagType(OpenCloseType In);
//OpenCloseType TThostFtdcOffsetFlagType_2_OpenCloseType(TThostFtdcOffsetFlagType In);

void OrderField_2_TBSType(OrderField* pIn, PSTOrder pOut);
OrderSide TBSFLAG_2_OrderSide(TBSFLAG In);
void OrderField_2_TMarketOrderFlagType(OrderField* pIn, PSTOrder pOut);
TMarketCodeType OrderField_2_TMarketCodeType(OrderField* pIn);

//OrderSide TThostFtdcDirectionType_2_OrderSide(TThostFtdcDirectionType In);
//PositionSide TThostFtdcPosiDirectionType_2_PositionSide(TThostFtdcPosiDirectionType In);
//PositionSide TradeField_2_PositionSide(TradeField* pIn);
//
//TThostFtdcOrderPriceTypeType OrderType_2_TThostFtdcOrderPriceTypeType(OrderType In);
//
//OrderType CThostFtdcOrderField_2_OrderType(CThostFtdcOrderField* pIn);
//TimeInForce CThostFtdcOrderField_2_TimeInForce(CThostFtdcOrderField* pIn);
//
OrderStatus TOrderStatusType_2_OrderStatus(TOrderStatusType In);
ExecType TOrderStatusType_2_ExecType(TOrderStatusType In);
//
//OrderStatus CThostFtdcQuoteField_2_OrderStatus(CThostFtdcQuoteField* pIn);
//ExecType CThostFtdcQuoteField_2_ExecType(CThostFtdcQuoteField* pIn);
//
//InstrumentType CThostFtdcInstrumentField_2_InstrumentType(CThostFtdcInstrumentField* pIn);
