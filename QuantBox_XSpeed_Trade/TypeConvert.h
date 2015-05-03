#pragma once

#include "../include/XSpeed/DFITCApiDataType.h"
#include "../include/XSpeed/DFITCApiStruct.h"
#include "../include/ApiStruct.h"


//PutCall TThostFtdcOptionsTypeType_2_PutCall(TThostFtdcOptionsTypeType In);

HedgeFlagType DFITCSpeculatorType_2_HedgeFlagType(DFITCSpeculatorType In);
DFITCSpeculatorType HedgeFlagType_2_DFITCSpeculatorType(HedgeFlagType In);

DFITCOpenCloseTypeType OpenCloseType_2_DFITCOpenCloseTypeType(OpenCloseType In);
OpenCloseType DFITCOpenCloseTypeType_2_OpenCloseType(DFITCOpenCloseTypeType In);

DFITCBuySellTypeType OrderSide_2_DFITCBuySellTypeType(OrderSide In);
OrderSide DFITCBuySellTypeType_2_OrderSide(DFITCBuySellTypeType In);

DFITCOrderTypeType OrderType_2_DFITCOrderTypeType(OrderType In);

TimeInForce DFITCOrderPropertyType_2_TimeInForce(DFITCOrderPropertyType In);
DFITCOrderPropertyType TimeInForce_2_DFITCOrderPropertyType(TimeInForce In);

OrderStatus DFITCOrderRtnField_2_OrderStatus(DFITCOrderRtnField* pIn);
OrderType DFITCOrderRtnField_2_OrderType(DFITCOrderRtnField* pIn);
TimeInForce DFITCOrderRtnField_2_TimeInForce(DFITCOrderRtnField* pIn);
ExecType DFITCOrderRtnField_2_ExecType(DFITCOrderRtnField* pIn);

InstrumentType DFITCInstrumentTypeType_2_InstrumentType(DFITCInstrumentTypeType In);

ExchangeType DFITCExchangeIDType_2_ExchangeType(DFITCExchangeIDType In);
