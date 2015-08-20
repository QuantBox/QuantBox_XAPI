/////////////////////////////////////////////////////////////////////////
///@system 新一代交易系统
///@company SunGard China
///@file KSVocApiStruct.h
///@brief 定义了客户端接口使用的业务数据结构
/////////////////////////////////////////////////////////////////////////

#ifndef __KSVOCAPISTRUCT_H_INCLUDED_
#define __KSVOCAPISTRUCT_H_INCLUDED_

#include "KSUserApiDataTypeEx.h"
#include "KSVocApiDataType.h"
#include "KSUserApiStructEx.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

namespace KingstarAPI
{

	///初始录入条件单
	struct CKSConditionalOrderInitInsert
	{
		///营业部代码
		TThostFtdcBrokerIDType BrokerID;
		///投资者代码
		TThostFtdcInvestorIDType InvestorID;
		///合约代码
		TThostFtdcInstrumentIDType	InstrumentID;
		///交易所代码
		TThostFtdcExchangeIDType	ExchangeID;
		///客户代码
		TThostFtdcClientIDType	ClientID;
		///买卖方向
		TThostFtdcDirectionType	Direction;
		///开平标志
		TThostFtdcOffsetFlagType	CombOffsetFlag;
		///投机套保标志
		TThostFtdcHedgeFlagType	CombHedgeFlag;
		///数量
		TThostFtdcVolumeType	VolumeTotalOriginal;
		///价格
		TThostFtdcPriceType	LimitPrice;
		///价格类别
		TKSOrderPriceTypeType OrderPriceType;             
		///条件类型
		TKSConditionalTypeType  ConditionalType;
		///条件价
		TThostFtdcPriceType ConditionalPrice;
		///条件单编号
		TKSConditionalOrderIDType	ConditionalOrderID;
		///触发次数
		TThostFtdcVolumeType TriggeredTimes;
		///条件单类型
		TKSConditionalOrderType OrderType;
		///有效时间
		TThostFtdcTimeType  ActiveTime;
		///失效时间
		TThostFtdcTimeType InActiveTime;
		///货币代码
		TThostFtdcCurrencyIDType    CurrencyID;
	};

	///修改条件单
	struct CKSConditionalOrderModify
	{
		///营业部代码
		TThostFtdcBrokerIDType BrokerID;
		///投资者代码
		TThostFtdcInvestorIDType InvestorID;
		///合约代码
		TThostFtdcInstrumentIDType	InstrumentID;
		///交易所代码
		TThostFtdcExchangeIDType	ExchangeID;
		///客户代码
		TThostFtdcClientIDType	ClientID;
		///买卖方向
		TThostFtdcDirectionType	Direction;
		///开平标志
		TThostFtdcOffsetFlagType	CombOffsetFlag;
		///投机套保标志
		TThostFtdcHedgeFlagType	CombHedgeFlag;
		///数量
		TThostFtdcVolumeType	VolumeTotalOriginal;
		///价格
		TThostFtdcPriceType	LimitPrice;
		///价格类别
		TKSOrderPriceTypeType OrderPriceType;          
		///条件类型
		TKSConditionalTypeType  ConditionalType;
		///条件价
		TThostFtdcPriceType ConditionalPrice;
		///条件单编号
		TKSConditionalOrderIDType	ConditionalOrderID;
		///触发次数
		TThostFtdcVolumeType TriggeredTimes;
		///条件单类型
		TKSConditionalOrderType OrderType;
		///有效时间
		TThostFtdcTimeType  ActiveTime;
		///失效时间
		TThostFtdcTimeType InActiveTime;
		///货币代码
		TThostFtdcCurrencyIDType    CurrencyID;
	};

	///查询条件单
	struct CKSConditionalOrderQuery
	{
		///营业部代码
		TThostFtdcBrokerIDType BrokerID;
		///投资者代码
		TThostFtdcInvestorIDType InvestorID;
		///条件单编号
		TKSConditionalOrderIDType	ConditionalOrderID;
	};

	///删除条件单
	struct CKSConditionalOrderRemove
	{
		///营业部代码
		TThostFtdcBrokerIDType BrokerID;
		///投资者代码
		TThostFtdcInvestorIDType InvestorID;
		///条件单编号
		TKSConditionalOrderIDType	ConditionalOrderID;
	};

	///暂停或激活条件单
	struct CKSConditionalOrderStateAlter
	{
		///营业部代码
		TThostFtdcBrokerIDType BrokerID;
		///投资者代码
		TThostFtdcInvestorIDType InvestorID;
		///条件单编号
		TKSConditionalOrderIDType	ConditionalOrderID;
		///暂停或激活操作标志
		TKSConditionalOrderStateAlterType	ConditionalOrderStateAlter;
	};

	///选择条件单
	struct CKSConditionalOrderSelect
	{
		///营业部代码
		TThostFtdcBrokerIDType BrokerID;
		///投资者代码
		TThostFtdcInvestorIDType InvestorID;
		///条件单编号
		TKSConditionalOrderIDType	ConditionalOrderID;
		///选择结果
		TKSConditionalOrderSelectResultType SelectResult;
	};


	///条件单处理结果信息
	struct CKSConditionalOrderRspResultField
	{
		///营业部代码
		TThostFtdcBrokerIDType BrokerID;
		///投资者代码
		TThostFtdcInvestorIDType InvestorID;
		///条件单编号
		TKSConditionalOrderIDType	ConditionalOrderID;
	};

	///条件单响应
	struct CKSConditionalOrderOperResultField
	{
		///营业部代码
		TThostFtdcBrokerIDType BrokerID;
		///投资者代码
		TThostFtdcInvestorIDType InvestorID;
		///条件单编号
		TKSConditionalOrderIDType	ConditionalOrderID;
		///本地报单编号
		TThostFtdcOrderLocalIDType	OrderLocalID;
		///交易所代码
		TThostFtdcExchangeIDType	ExchangeID;
		///合约代码
		TThostFtdcInstrumentIDType	InstrumentID;
		///报单状态
		TThostFtdcOrderStatusType	OrderStatus;
		///开平标志
		TThostFtdcOffsetFlagType	CombOffsetFlag;
		///投机套保标志
		TThostFtdcHedgeFlagType	CombHedgeFlag;
		///买卖方向
		TThostFtdcDirectionType	Direction;
		///价格
		TThostFtdcPriceType	LimitPrice;
		///数量
		TThostFtdcVolumeType	VolumeTotalOriginal;
		///交易日
		TThostFtdcTradeDateType	TradingDay;
		///撤销用户
		TThostFtdcUserIDType	UserID;
		///撤销时间
		TThostFtdcTimeType	CancelTime;
		///客户代码
		TThostFtdcClientIDType	ClientID;
		///条件单状态
		TKSConditionalOrderStatusType ConditionalOrderStatus;
		///错误信息
		TThostFtdcErrorMsgType	ErrorMsg;
		///价格类别
		TThostFtdcOrderPriceTypeType OrderPriceType;
		///触发次数
		TThostFtdcVolumeType TriggeredTimes;
		///条件单类型
		TKSConditionalOrderType OrderType;
		///备注
		TThostFtdcMemoType	Memo;
		///有效时间
		TThostFtdcTimeType  ActiveTime;
		///失效时间
		TThostFtdcTimeType InActiveTime;
		///条件类型
		TKSConditionalTypeType  ConditionalType;
		///条件价
		TThostFtdcPriceType ConditionalPrice;
	};

	///录入止损止盈单
	struct CKSProfitAndLossOrderInsert
	{
		///营业部代码
		TThostFtdcBrokerIDType BrokerID;
		///投资者代码
		TThostFtdcInvestorIDType InvestorID;
		///本地报单编号
		TThostFtdcOrderLocalIDType	OrderLocalID;
		///止损价
		TThostFtdcPriceType  StopLossPrice;
		///止盈价
		TThostFtdcPriceType  TakeProfitPrice;
		///平仓方式
		TKSCloseModeType CloseMode;
		//平仓反向加减价位数（平仓方式=1或2时有效）
		TThostFtdcPriceType FiguresPrice;
		///交易所代码
		TThostFtdcExchangeIDType	ExchangeID;
		///业务单元
		TThostFtdcBusinessUnitType	BusinessUnit;
		///生成盈损价的方式
		TKSOffsetValueType OffsetValue;
		///条件单触发价格类型
		TKSSpringTypeType SpringType;
		///浮动止损价
		TThostFtdcPriceType	FloatLimitPrice;
		///止损单触发次数
		TThostFtdcVolumeType TriggeredTimes;
		///货币代码
		TThostFtdcCurrencyIDType    CurrencyID;
		///合约代码
		TThostFtdcInstrumentIDType	InstrumentID;
		///止损止盈标志
		TKSProfitAndLossFlagType	ProfitAndLossFlag;
	};

	///删除止损止盈单
	struct CKSProfitAndLossOrderRemove
	{
		///营业部代码
		TThostFtdcBrokerIDType BrokerID;
		///投资者代码
		TThostFtdcInvestorIDType InvestorID;
		///止损止盈单号
		TKSProfitAndLossOrderIDType ProfitAndLossOrderID;
		///本地报单编号
		TThostFtdcOrderLocalIDType	OrderLocalID;
		///交易所代码
		TThostFtdcExchangeIDType	ExchangeID;
		///业务单元
		TThostFtdcBusinessUnitType	BusinessUnit;
	};

	///查询止损止盈单
	struct CKSProfitAndLossOrderQuery
	{
		///营业部代码
		TThostFtdcBrokerIDType BrokerID;
		///投资者代码
		TThostFtdcInvestorIDType InvestorID;
		///止损止盈单号
		TKSProfitAndLossOrderIDType ProfitAndLossOrderID;
		///本地报单编号
		TThostFtdcOrderLocalIDType	OrderLocalID;
		///交易所代码
		TThostFtdcExchangeIDType	ExchangeID;
		///业务单元
		TThostFtdcBusinessUnitType	BusinessUnit;
	};

	///修改止损止盈单
	struct CKSProfitAndLossOrderModify
	{
		///营业部代码
		TThostFtdcBrokerIDType BrokerID;
		///投资者代码
		TThostFtdcInvestorIDType InvestorID;
		///止损止盈单号
		TKSProfitAndLossOrderIDType ProfitAndLossOrderID;
		///止损价
		TThostFtdcPriceType  StopLossPrice;
		///止盈价
		TThostFtdcPriceType  TakeProfitPrice;
		///平仓方式
		TKSCloseModeType CloseMode;
		//平仓反向价位数（平仓方式=1或2时有效）
		TThostFtdcPriceType FiguresPrice;
		///生成盈损价的方式
		TKSOffsetValueType OffsetValue;
		///条件单触发价格类型
		TKSSpringTypeType SpringType;
		///浮动止损价
		TThostFtdcPriceType	FloatLimitPrice;
		///触发次数
		TThostFtdcVolumeType TriggeredTimes;
		///止损止盈标志
		TKSProfitAndLossFlagType	ProfitAndLossFlag;
	};

	///止损止盈单删除
	struct CKSProfitAndLossOrderRemoveField
	{
		///营业部代码
		TThostFtdcBrokerIDType BrokerID;
		///投资者代码
		TThostFtdcInvestorIDType InvestorID;
		///止损止盈单号
		TKSProfitAndLossOrderIDType ProfitAndLossOrderID;
	};

	///止损止盈单处理
	struct CKSProfitAndLossOrderOperResultField
	{
		///营业部代码
		TThostFtdcBrokerIDType BrokerID;
		///投资者代码
		TThostFtdcInvestorIDType InvestorID;
		///止损止盈单号
		TKSProfitAndLossOrderIDType ProfitAndLossOrderID;
		///操作员
		TThostFtdcUserIDType	UserID;
		///投资者名称
		TThostFtdcPartyNameType InvestorName;
		///本地报单编号
		TThostFtdcOrderLocalIDType	OrderLocalID;
		///止损价
		TThostFtdcPriceType  StopLossPrice;
		///止盈价
		TThostFtdcPriceType  TakeProfitPrice;
		///平仓方式
		TKSCloseModeType CloseMode;
		///平仓反向加减价位数（平仓方式=1或2时有效）
		TThostFtdcPriceType Figures;
		///行情触发时的最新价
		TThostFtdcPriceType LastPrice;
		///生成止损止盈单时间
		TThostFtdcTimeType	ProfitAndLossOrderFormTime;
		///生成条件单时间
		TThostFtdcTimeType	ConditionalOrderFormTime;
		///生成委托单时间
		TThostFtdcTimeType	OrderFormTime;
		///止损止盈单状态
		TKSConditionalOrderStatusType ProfitAndLossOrderStatus;
		///条件单编号
		TKSConditionalOrderIDType	ConditionalOrderID;
		///交易所代码
		TThostFtdcExchangeIDType	ExchangeID;
		///客户代码
		TThostFtdcClientIDType	ClientID;
		///合约代码
		TThostFtdcInstrumentIDType	InstrumentID;
		///开平标志
		TThostFtdcOffsetFlagType	CombOffsetFlag;
		///投机套保标志
		TThostFtdcHedgeFlagType	CombHedgeFlag;
		///买卖方向
		TThostFtdcDirectionType	Direction;
		///价格
		TThostFtdcPriceType	LimitPrice;
		///数量
		TThostFtdcVolumeType	VolumeTotalOriginal;
		///生成盈损价的方式
		TKSOffsetValueType OffsetValue;
		///业务单元
		TThostFtdcBusinessUnitType	BusinessUnit;
		///条件单触发价格类型
		TKSSpringTypeType SpringType;
		///浮动止损价
		TThostFtdcPriceType	FloatLimitPrice;
		///开仓成交价格
		TThostFtdcPriceType OpenTradePrice;
		///止损止盈标志
		TKSProfitAndLossFlagType	ProfitAndLossFlag;
	};

	// 条件单请求选择信息
	struct CKSCOSAskSelectField
	{
		///营业部代码
		TThostFtdcBrokerIDType BrokerID;   
		///操作员
		TThostFtdcUserIDType	UserID;
		///投资者代码
		TThostFtdcInvestorIDType InvestorID;
		///序号
		TThostFtdcSequenceNoType	SequenceNo;
		///条件单编号
		TKSConditionalOrderIDType	ConditionalOrderID;
		///备注
		TThostFtdcMemoType	Memo;
		///选择方式
		TKSConditionalOrderSelectTypeType SelectType;
	};

	// 条件单状态信息
	struct CKSCOSStatusField
	{
		///营业部代码
		TThostFtdcBrokerIDType BrokerID;   
		///操作员
		TThostFtdcUserIDType	UserID;
		///投资者代码
		TThostFtdcInvestorIDType InvestorID;
		///序号
		TThostFtdcSequenceNoType	SequenceNo;
		///条件单编号
		TKSConditionalOrderIDType	ConditionalOrderID;
		///条件单状态
		TKSConditionalOrderStatusType ConditionalOrderStatus;
		///备注
		TThostFtdcMemoType	Memo;
		///本地报单编号
		TThostFtdcOrderLocalIDType	OrderLocalID;
		///交易所代码
		TThostFtdcExchangeIDType	ExchangeID;
		///合约代码
		TThostFtdcInstrumentIDType	InstrumentID;
		///报单状态
		TThostFtdcOrderStatusType	OrderStatus;
		///开平标志
		TThostFtdcOffsetFlagType	CombOffsetFlag;
		///投机套保标志
		TThostFtdcHedgeFlagType	CombHedgeFlag;
		///买卖方向
		TThostFtdcDirectionType	Direction;
		///价格
		TThostFtdcPriceType	LimitPrice;
		///数量
		TThostFtdcVolumeType	VolumeTotalOriginal;
		///交易日
		TThostFtdcTradeDateType	TradingDay;
		///撤销用户
		TThostFtdcUserIDType	CancelUserID;
		///撤销时间
		TThostFtdcTimeType	CancelTime;
		///客户代码
		TThostFtdcClientIDType	ClientID;
		///业务单元
		TThostFtdcBusinessUnitType	BusinessUnit;
		///报单编号
		TThostFtdcOrderSysIDType	OrderSysID;
		///今成交数量
		TThostFtdcVolumeType	VolumeTraded;
		///剩余数量
		TThostFtdcVolumeType	VolumeTotal;
		///委托时间
		TThostFtdcTimeType	InsertTime;
		///激活时间
		TThostFtdcTimeType	ActiveTime;
		///成交价格
		TThostFtdcPriceType	TradePrice;
		///货币代码
		TThostFtdcCurrencyIDType    CurrencyID;
	};

	// 止损止盈单状态信息
	struct CKSPLStatusField
	{
		///营业部代码
		TThostFtdcBrokerIDType BrokerID;   
		///操作员
		TThostFtdcUserIDType	UserID;
		///投资者代码
		TThostFtdcInvestorIDType InvestorID;
		///序号
		TThostFtdcSequenceNoType	SequenceNo;
		///止损止盈单编号
		TKSProfitAndLossOrderIDType	ProfitAndLossOrderID;
		///止损条件单编号
		TKSConditionalOrderIDType	StopLossOrderID;
		///止盈条件单编号
		TKSConditionalOrderIDType	TakeProfitOrderID;
		///盈损单状态
		TKSConditionalOrderStatusType ProfitAndLossOrderStatus;
		///止损价
		TThostFtdcPriceType  StopLossPrice;
		///止盈价
		TThostFtdcPriceType  TakeProfitPrice;
		///生成盈损价的方式
		TKSOffsetValueType OffsetValue;
		///开仓成交价格
		TThostFtdcPriceType OpenTradePrice;
		///备注
		TThostFtdcMemoType	Memo;
		///本地报单编号
		TThostFtdcOrderLocalIDType	OrderLocalID;
		///交易所代码
		TThostFtdcExchangeIDType	ExchangeID;
		///合约代码
		TThostFtdcInstrumentIDType	InstrumentID;
		///报单状态
		TThostFtdcOrderStatusType	OrderStatus;
		///开平标志
		TThostFtdcOffsetFlagType	CombOffsetFlag;
		///投机套保标志
		TThostFtdcHedgeFlagType	CombHedgeFlag;
		///买卖方向
		TThostFtdcDirectionType	Direction;
		///价格
		TThostFtdcPriceType	LimitPrice;
		///数量
		TThostFtdcVolumeType	VolumeTotalOriginal;
		///交易日
		TThostFtdcTradeDateType	TradingDay;
		///撤销用户
		TThostFtdcUserIDType	CancelUserID;
		///撤销时间
		TThostFtdcTimeType	CancelTime;
		///客户代码
		TThostFtdcClientIDType	ClientID;
		///业务单元
		TThostFtdcBusinessUnitType	BusinessUnit;
		///报单编号
		TThostFtdcOrderSysIDType	OrderSysID;
		///今成交数量
		TThostFtdcVolumeType	VolumeTraded;
		///剩余数量
		TThostFtdcVolumeType	VolumeTotal;
		///委托时间
		TThostFtdcTimeType	InsertTime;
		///激活时间
		TThostFtdcTimeType	ActiveTime;
		///成交价格
		TThostFtdcPriceType	TradePrice;
		///货币代码
		TThostFtdcCurrencyIDType    CurrencyID;
	};

	///输入查询现货指数
	struct CKSQryIndexPriceField
	{
		///经纪公司代码
		TThostFtdcBrokerIDType	BrokerID;
		///交易所代码
		TThostFtdcExchangeIDType	ExchangeID;
		///合约代码
		TThostFtdcInstrumentIDType	InstrumentID;
	};

	///输出查询现货指数
	struct CKSIndexPriceField
	{
		///经纪公司代码
		TThostFtdcBrokerIDType	BrokerID;
		///交易所代码
		TThostFtdcExchangeIDType	ExchangeID;
		///合约代码
		TThostFtdcInstrumentIDType	InstrumentID;
		///指数现货收盘价
		TThostFtdcPriceType	ClosePrice;
		///安全系数
		TThostFtdcRatioType	GuarantRatio;
	};

	///输入查询期权合约保障系数
	struct CKSQryOptionInstrGuardField
	{
		///经纪公司代码
		TThostFtdcBrokerIDType	BrokerID;
		///交易所代码
		TThostFtdcExchangeIDType	ExchangeID;
		///合约代码
		TThostFtdcInstrumentIDType	InstrumentID;
	};

	///输出查询期权合约保障系数
	struct CKSOptionInstrGuardField
	{
		///经纪公司代码
		TThostFtdcBrokerIDType	BrokerID;
		///交易所代码
		TThostFtdcExchangeIDType	ExchangeID;
		///合约代码
		TThostFtdcInstrumentIDType	InstrumentID;
		///最低保障系数分子
		TThostFtdcVolumeType	MiniNumerator;
		///最低保障系数分母
		TThostFtdcVolumeType	MiniDenominator ;
		///期权合约最小保证金
		TThostFtdcMoneyType	MiniMargin;
	};

	///输入查询宣告数量
	struct CKSQryExecOrderVolumeField
	{
		///经纪公司代码
		TThostFtdcBrokerIDType	BrokerID;
		///投资者代码
		TThostFtdcInvestorIDType	InvestorID;
		///交易所代码
		TThostFtdcExchangeIDType	ExchangeID;
		///合约代码
		TThostFtdcInstrumentIDType	InstrumentID;
		///产品代码
		TThostFtdcInstrumentIDType	ProductID;
		///投机套保标志
		TThostFtdcHedgeFlagType	HedgeFlag;
		///买卖标志
		TThostFtdcDirectionType	Direction;
	};

	///输出查询宣告数量
	struct CKSExecOrderVolumeField
	{
		///经纪公司代码
		TThostFtdcBrokerIDType	BrokerID;
		///投资者代码
		TThostFtdcInvestorIDType	InvestorID;
		///合约代码
		TThostFtdcInstrumentIDType	InstrumentID;
		///产品代码
		TThostFtdcInstrumentIDType	ProductID;
		///投机套保标志
		TThostFtdcHedgeFlagType	HedgeFlag;
		///可申请执行量
		TThostFtdcVolumeType	ExecVolume;
		///可申请放弃量
		TThostFtdcVolumeType	ActionVolume;
		///已申请执行量
		TThostFtdcVolumeType	ExecedVolume;
		///已申请放弃量 
		TThostFtdcVolumeType	ActionedVolume;
		///买卖标志
		TThostFtdcDirectionType	Direction;
	};

	///输入个股行权指派信息
	struct CKSQryStockOptionAssignmentField
	{
		///经纪公司代码
		TThostFtdcBrokerIDType	BrokerID;
		///投资者代码
		TThostFtdcInvestorIDType	InvestorID;
		///交易所代码
		TThostFtdcExchangeIDType	ExchangeID;
		///合约代码
		TThostFtdcInstrumentIDType	InstrumentID;
		///买卖
		TThostFtdcDirectionType	Direction;
	};

	///输出个股行权指派信息
	struct CKSStockOptionAssignmentField
	{
		///经纪公司代码
		TThostFtdcBrokerIDType	BrokerID;
		///投资者代码
		TThostFtdcInvestorIDType	InvestorID;
		///合约代码
		TThostFtdcInstrumentIDType	InstrumentID;
		///买卖
		TThostFtdcDirectionType	Direction;
		///交易所代码
		TThostFtdcExchangeIDType	ExchangeID;
		///合约名称
		TThostFtdcInstrumentNameType	InstrumentName;
		///产品代码
		TThostFtdcInstrumentIDType	ProductID;
		///投机套保标志
		TThostFtdcHedgeFlagType	HedgeFlag;
		///上日持仓
		TThostFtdcVolumeType	YdPosition;
		///行权指派合约数量
		TThostFtdcVolumeType AssInsVo;
		///行权标的证券数量
		TThostFtdcVolumeType AssProVol;
		///行权指派应付金额
		TThostFtdcMoneyType  FeePay;
		///期权C/P标志
		TThostFtdcOptionsTypeType OptionsType;
		///行权交收日
		TThostFtdcDateType DeliveryDay;
		///个股合约标识码
		TThostFtdcInstrumentIDType StockID;
	};

	///报单Key
	struct CThostOrderKeyField
	{
		///报单引用
		TThostFtdcOrderRefType      OrderRef;
		///前置编号
		TThostFtdcFrontIDType  FrontID;
		///会话编号
		TThostFtdcSessionIDType     SessionID;
		///交易所代码
		TThostFtdcExchangeIDType  ExchangeID;
		///报单编号
		TThostFtdcOrderSysIDType  OrderSysID;
	};

	///输入批量报单操作
	struct CThostFtdcBulkCancelOrderField
	{
		///经纪公司代码
		TThostFtdcBrokerIDType      BrokerID;
		///投资者代码
		TThostFtdcInvestorIDType    InvestorID;
		///用户代码
		TThostFtdcUserIDType  UserID;
		///报单类型
		TThostFtdcOrderTypeType    OrderType;
		///报单个数
		TThostFtdcVolumeType nCount;
		///报单集合
		CThostOrderKeyField OrderKey[MAX_ORDER_COUNT];
	};

	///平仓策略
	struct CKSCloseStrategy
	{
		///营业部代码
		TThostFtdcBrokerIDType BrokerID;
		///投资者代码
		TThostFtdcInvestorIDType InvestorID;
	};

	///平仓策略响应
	struct CKSCloseStrategyResultField
	{
		///交易所代码
		TThostFtdcExchangeIDType	ExchangeID;
		///交易所名称
		TThostFtdcExchangeNameType	ExchangeName;
		///平仓策略
		TKSCloseStrategyType CloseStrategy;
	};

	///组合策略
	struct CKSCombStrategy
	{
		///营业部代码
		TThostFtdcBrokerIDType BrokerID;
		///投资者代码
		TThostFtdcInvestorIDType InvestorID;
		///交易类别(1:期货;2:期权;空:全部)
		TThostFtdcProductClassType	ProductClass;
		///交易所代码
		TThostFtdcExchangeIDType	ExchangeID;
		///策略代码(SP/SPD)
		TKSStrategyIDType StrategyID;
		///组合类型
		TKSCombTypeType CombType;
	};

	///组合策略响应
	struct CKSCombStrategyResultField
	{
		///交易所代码
		TThostFtdcExchangeIDType	ExchangeID;
		///策略代码
		TKSStrategyIDType	StrategyID;
		///组合类型
		TKSCombTypeType CombType;
		///组合买卖方向
		TThostFtdcDirectionType	CombDirection;
		///组合开平标志
		TThostFtdcCombOffsetFlagType	CombOffsetFlag;
		///数量
		TThostFtdcVolumeType	Volume;
		///买卖方向一
		TThostFtdcDirectionType	Direction1;
		///开平标志一
		TThostFtdcOffsetFlagType	OffsetFlag1;
		///数量比例一
		TThostFtdcVolRatioType	VolRatio1;
		///计算符号一
		TKSCalcFlagType CalcFlag1;
		///保证金比例一
		TThostFtdcMoneyRatioType	MarginRatio1;
		///买卖方向二
		TThostFtdcDirectionType	Direction2;
		///开平标志二
		TThostFtdcOffsetFlagType	OffsetFlag2;
		///数量比例二
		TThostFtdcVolRatioType	VolRatio2;
		///计算符号二
		TKSCalcFlagType CalcFlag2;
		///保证金比例二
		TThostFtdcMoneyRatioType	MarginRatio2;
		///买卖方向三
		TThostFtdcDirectionType	Direction3;
		///开平标志三
		TThostFtdcOffsetFlagType	OffsetFlag3;
		///数量比例三
		TThostFtdcVolRatioType	VolRatio3;
		///计算符号三
		TKSCalcFlagType CalcFlag3;
		///保证金比例三
		TThostFtdcMoneyRatioType	MarginRatio3;
		///买卖方向四
		TThostFtdcDirectionType	Direction4;
		///开平标志四
		TThostFtdcOffsetFlagType	OffsetFlag4;
		///数量比例四
		TThostFtdcVolRatioType	VolRatio4;
		///计算符号四
		TKSCalcFlagType CalcFlag4;
		///保证金比例四
		TThostFtdcMoneyRatioType	MarginRatio4;
		///组合合约代码
		TThostFtdcInstrumentIDType	CombInstrumentID;
	};

	///期权组合策略
	struct CKSOptionCombStrategy
	{
		///营业部代码
		TThostFtdcBrokerIDType BrokerID;
		///投资者代码
		TThostFtdcInvestorIDType InvestorID;
		///交易所代码
		TThostFtdcExchangeIDType	ExchangeID;
		///策略代码
		TKSStrategyIDType	StrategyID;
		///组合买卖方向
		TThostFtdcDirectionType	CombDirection;
		///组合开平标志
		TThostFtdcCombOffsetFlagType	CombOffsetFlag;
	};

	///期权组合策略响应
	struct CKSOptionCombStrategyResultField
	{
		///交易所代码
		TThostFtdcExchangeIDType	ExchangeID;
		///策略代码
		TKSStrategyIDType	StrategyID;
		///组合买卖方向
		TThostFtdcDirectionType	CombDirection;
		///组合开平标志
		TThostFtdcCombOffsetFlagType	CombOffsetFlag;
		///数量
		TThostFtdcVolumeType	Volume;
		///买卖方向一
		TThostFtdcDirectionType	Direction1;
		///期权类型一
		TThostFtdcOptionsTypeType	OptionsType1;
		///执行价类型一
		TKSStrikePriceType StrikeType1;
		///开平标志一
		TThostFtdcOffsetFlagType	OffsetFlag1;
		///数量比例一
		TThostFtdcVolRatioType	VolRatio1;
		///保证金比例一
		TThostFtdcMoneyRatioType	MarginRatio1;
		///计算符号一
		TKSCalcFlagType CalcFlag1;
		///买卖方向二
		TThostFtdcDirectionType	Direction2;
		///期权类型二
		TThostFtdcOptionsTypeType	OptionsType2;
		///执行价类型二
		TKSStrikePriceType StrikeType2;
		///开平标志二
		TThostFtdcOffsetFlagType	OffsetFlag2;
		///数量比例二
		TThostFtdcVolRatioType	VolRatio2;
		///保证金比例二
		TThostFtdcMoneyRatioType	MarginRatio2;
		///计算符号二
		TKSCalcFlagType CalcFlag2;
		///买卖方向三
		TThostFtdcDirectionType	Direction3;
		///期权类型三
		TThostFtdcOptionsTypeType	OptionsType3;
		///执行价类型三
		TKSStrikePriceType StrikeType3;
		///开平标志三
		TThostFtdcOffsetFlagType	OffsetFlag3;
		///数量比例三
		TThostFtdcVolRatioType	VolRatio3;
		///保证金比例三
		TThostFtdcMoneyRatioType	MarginRatio3;
		///计算符号三
		TKSCalcFlagType CalcFlag3;
		///买卖方向四
		TThostFtdcDirectionType	Direction4;
		///期权类型四
		TThostFtdcOptionsTypeType	OptionsType4;
		///执行价类型四
		TKSStrikePriceType StrikeType4;
		///开平标志四
		TThostFtdcOffsetFlagType	OffsetFlag4;
		///数量比例四
		TThostFtdcVolRatioType	VolRatio4;
		///保证金比例四
		TThostFtdcMoneyRatioType	MarginRatio4;
		///计算符号四
		TKSCalcFlagType CalcFlag4;
	};

	///查询客户转帐信息请求
	struct CKSTransferInfo
	{
		///银行代码
		TThostFtdcBankIDType	BankID;
		///币种代码
		TThostFtdcCurrencyIDType	CurrencyID;
	};

	///查询客户转帐信息响应
	struct CKSTransferInfoResultField
	{
		///入金开始时间
		TThostFtdcTimeType	DepositBegin;
		///入金结束时间
		TThostFtdcTimeType	DepositEnd;
		///出金开始时间
		TThostFtdcTimeType	WithdrawBegin;
		///出金结束时间
		TThostFtdcTimeType	WithdrawEnd;
		///出金次数
		TThostFtdcLargeVolumeType	WithdrawVolume;
		///出金限额
		TThostFtdcMoneyType	WithdrawLimit;
		///出金单笔限额
		TThostFtdcMoneyType	WithdrawLimitByTrade;
		///可提资金
		TThostFtdcMoneyType	Available;
		///已出次数
		TThostFtdcLargeVolumeType	WithdrawedVolume;
		///已出金额
		TThostFtdcMoneyType	WithdrawedAmount;
		///币种代码
		TThostFtdcCurrencyIDType	CurrencyID;
	};

	///查询Kingstar交易事件通知
	struct CKSQryTradingNoticeField
	{
		///经纪公司代码
		TThostFtdcBrokerIDType	BrokerID;
		///投资者代码
		TThostFtdcInvestorIDType	InvestorID;
		///信息类型
		TKSInfoTypeType InfoType;
		///确认标志
		TKSConfirmFlagType ConfirmFlag;
		///开始时间
		TThostFtdcTimeType	TimeStart;
		///结束时间
		TThostFtdcTimeType	TimeEnd;
	};

	///用户事件通知
	struct CKSTradingNoticeField
	{
		///经纪公司代码
		TThostFtdcBrokerIDType	BrokerID;
		///投资者代码
		TThostFtdcInvestorIDType	InvestorID;
		///信息类型
		TKSInfoTypeType InfoType;
		///信息摘要
		TThostFtdcDigestType	InfoDigest;
		///消息正文
		TThostFtdcContentType	FieldContent;
		///发送日期
		TThostFtdcTimeType	SendDate;
		///发送时间
		TThostFtdcTimeType	SendTime;
		///序列号
		TThostFtdcSequenceNoType	SequenceNo;
		///确认标志
		TKSConfirmFlagType ConfirmFlag;
	};

	///用户端产品资源查询
	struct CKSQryUserProductUrlField
	{
		///经纪公司代码
		TThostFtdcBrokerIDType	BrokerID;
		///投资者代码
		TThostFtdcInvestorIDType	InvestorID;
		///用户端产品信息
		TThostFtdcProductInfoType	 UserProductInfo;
	};

	///用户端产品资源
	struct CKSUserProductUrlField
	{
		///经纪公司代码
		TThostFtdcBrokerIDType	BrokerID;
		///用户端产品信息
		TThostFtdcProductInfoType	 UserProductInfo;
		///用户端产品最新版本号
		TKSProductVersionType	 LastProductVersion;
		///用户端产品可用最低版本号
		TKSProductVersionType	 MinUseProductVersion;
		///用户端产品可升级最低版本号
		TKSProductVersionType	 MinUpdateVersion;
		///用户端产品最新版本安装包大小
		TThostFtdcLargeVolumeType  InstallPackSize;
		///用户端产品最新版本发布日期
		TThostFtdcTradeDateType	 PublishDate;
		///用户端产品最新版本说明
		TThostFtdcUserProductMemoType  ProductMemo;
		///用户端产品资源Url地址
		TThostFtdcFunctionUrlType Url;
	};

	///查询客户交易级别
	struct CKSQryInvestorTradeLevelField
	{
		///经纪公司代码
		TThostFtdcBrokerIDType BrokerID;
		///交易所代码
		TThostFtdcExchangeIDType ExchangeID;
		///产品代码
		TThostFtdcInstrumentIDType ProductID;
	};

	///客户交易级别
	struct CKSInvestorTradeLevelField
	{
		///经纪公司代码
		TThostFtdcBrokerIDType BrokerID;
		///投资者代码
		TThostFtdcInvestorIDType	InvestorID;
		///交易所代码
		TThostFtdcExchangeIDType ExchangeID;
		///产品代码
		TThostFtdcInstrumentIDType ProductID;
		///交易级别
		TKSTradeLevelType TradeLevel;
	};

	///查询个股限购额度
	struct CKSQryPurchaseLimitAmtField
	{
		///经纪公司代码
		TThostFtdcBrokerIDType BrokerID;
		///交易所代码
		TThostFtdcExchangeIDType ExchangeID;
	};

	///个股限购额度
	struct CKSPurchaseLimitAmtField
	{
		///经纪公司代码
		TThostFtdcBrokerIDType BrokerID;
		///投资者代码
		TThostFtdcInvestorIDType	InvestorID;
		///交易所代码
		TThostFtdcExchangeIDType ExchangeID;
		///限购额度
		TThostFtdcMoneyType	PurLimitAmt;
	};

	///查询个股限仓额度
	struct CKSQryPositionLimitVolField
	{
		///经纪公司代码
		TThostFtdcBrokerIDType BrokerID;
		///交易所代码
		TThostFtdcExchangeIDType ExchangeID;
		///产品代码
		TThostFtdcInstrumentIDType ProductID;
		///保护性标志
		TKSProtectFlagType ProtectFlag;
		///期权C/P标志
		TThostFtdcOptionsTypeType OptionsType;
		///控制范围
		TKSControlRangeType ControlRange;
	};

	///查询个股限仓额度
	struct CKSPositionLimitVolField
	{
		///经纪公司代码
		TThostFtdcBrokerIDType BrokerID;
		///投资者代码
		TThostFtdcInvestorIDType	InvestorID;
		///交易所代码
		TThostFtdcExchangeIDType ExchangeID;
		///产品代码
		TThostFtdcInstrumentIDType ProductID;
		///保护性标志
		TKSProtectFlagType ProtectFlag;
		///期权C/P标志
		TThostFtdcOptionsTypeType OptionsType;
		///控制范围
		TKSControlRangeType ControlRange;
		///限仓额度
		TThostFtdcVolumeType	PosiLimitVol;
	};

	///查询历史报单
	struct CKSQryHistoryOrderField
	{
		///经纪公司代码
		TThostFtdcBrokerIDType	BrokerID;
		///投资者代码
		TThostFtdcInvestorIDType	InvestorID;
		///合约代码
		TThostFtdcInstrumentIDType	InstrumentID;
		///本地报单编号
		TThostFtdcOrderLocalIDType	OrderLocalID;
		///品种代码
		TThostFtdcInstrumentIDType	ProductID;
		///开始日期
		TThostFtdcDateType	OrderDataStart;
		///结束日期
		TThostFtdcDateType	OrderDataEnd;
	};

	///查询历史成交
	struct CKSQryHistoryTradeField
	{
		///经纪公司代码
		TThostFtdcBrokerIDType	BrokerID;
		///投资者代码
		TThostFtdcInvestorIDType	InvestorID;
		///品种代码
		TThostFtdcInstrumentIDType	ProductID;
		///开始日期
		TThostFtdcDateType	TradeDataStart;
		///结束日期
		TThostFtdcDateType	TradeDataEnd;
	};

	///历史报单
	struct CKSHistoryOrderField
	{
		///经纪公司代码
		TThostFtdcBrokerIDType	BrokerID;
		///投资者代码
		TThostFtdcInvestorIDType	InvestorID;
		///客户姓名
		TThostFtdcIndividualNameType	CustomerName;
		///合约代码
		TThostFtdcInstrumentIDType	InstrumentID;
		///本地报单编号
		TThostFtdcOrderLocalIDType	OrderLocalID;
		///申报时间
		TThostFtdcTimeType	InsertTime;
		///业务单元
		TThostFtdcBusinessUnitType	BusinessUnit;
		///成交价格
		TThostFtdcPriceType	TradePrice;
		///成交金额
		TThostFtdcMoneyType	TradeAmount;
		///成交数量
		TThostFtdcVolumeType	VolumeTraded;
		///冻结解冻金额
		TThostFtdcMoneyType	FrozenAmount;
		///币种代码
		TThostFtdcCurrencyIDType	CurrencyID;
		///序号
		TThostFtdcSequenceNoType	SequenceNo;
		///买卖方向
		TThostFtdcDirectionType	Direction;
		///交易所代码
		TThostFtdcExchangeIDType	ExchangeID;
		///用户端产品信息
		TThostFtdcProductInfoType	UserProductInfo;
		///请求编号
		TThostFtdcRequestIDType	RequestID;
		///报单引用
		TThostFtdcOrderRefType	OrderRef;
		///前置编号
		TThostFtdcFrontIDType	FrontID;
		///会话编号
		TThostFtdcSessionIDType	SessionID;
		///委托价格
		TThostFtdcPriceType	OrderPrice;
		///委托来源
		TThostFtdcOrderSourceType	OrderSource;
		///委托日期
		TThostFtdcDateType	InsertDate;
		///委托时间
		TThostFtdcTimeType	OrderTime;
		///委托数量
		TThostFtdcVolumeType	VolumeTotalOriginal;
		///委托状态
		TThostFtdcOrderStatusType	OrderStatus;
		///产品代码
		TThostFtdcInstrumentIDType	ProductID;
		///产品名称
		TThostFtdcProductNameType	ProductName;
		///产品类型
		TThostFtdcProductClassType	ProductClass;
		///投保买卖开平标志
		TThostFtdcOffsetFlagType	OffsetFlag;
		///业务类型名称
		TThostFtdcFunctionNameType	FunctionName;
	};

	///历史成交
	struct CKSHistoryTradeField
	{
		///经纪公司代码
		TThostFtdcBrokerIDType	BrokerID;
		///投资者代码
		TThostFtdcInvestorIDType	InvestorID;
		///客户姓名
		TThostFtdcIndividualNameType	CustomerName;
		///本地报单编号
		TThostFtdcOrderLocalIDType	OrderLocalID;
		///币种代码
		TThostFtdcCurrencyIDType	CurrencyID;
		///交易所代码
		TThostFtdcExchangeIDType	ExchangeID;
		///产品代码
		TThostFtdcInstrumentIDType	ProductID;
		///产品名称
		TThostFtdcProductNameType	ProductName;
		///成交数量
		TThostFtdcVolumeType	VolumeTraded;
		///成交金额
		TThostFtdcMoneyType	TradeAmount;
		///成交日期
		TThostFtdcDateType	TradeDate;
		///成交时间
		TThostFtdcTimeType	TradeTime;
		///投保买卖开平标志
		TThostFtdcOffsetFlagType	OffsetFlag;
		///业务单元
		TThostFtdcBusinessUnitType	BusinessUnit;
		///手续费
		TThostFtdcMoneyType	Commission;
		///备注
		TThostFtdcMemoType	Memo;
		///报盘时间
		TThostFtdcTimeType	TraderOfferTime;
		///成交价格
		TThostFtdcPriceType	TradePrice;	
		///期权交易编码
		TThostFtdcClientIDType	ClientID;
		///期权类型
		TThostFtdcOptionsTypeType	OptionsType;
		///备兑标志
		TThostFtdcHedgeFlagType	HedgeFlag;
		///权利仓数量
		TThostFtdcVolumeType RoyaltyVolume;
		///义务仓数量
		TThostFtdcVolumeType ObligationVolume;
		///权利仓金额
		TThostFtdcMoneyType	RoyaltyAmount;
		///义务仓金额
		TThostFtdcMoneyType	ObligationAmount;
		///成交编号
		TThostFtdcTradeIDType	TradeID;
		///业务类型名称
		TThostFtdcFunctionNameType	FunctionName;
	};

	///最大组合拆分单量查询请求
	struct CKSQryMaxCombActionVolumeField
	{
		///经纪公司代码
		TThostFtdcBrokerIDType	BrokerID;
		///投资者代码
		TThostFtdcInvestorIDType	InvestorID;
		///交易所代码
		TThostFtdcExchangeIDType	ExchangeID;
		///合约代码
		TThostFtdcInstrumentIDType	InstrumentID;
		///投保标记
		TThostFtdcHedgeFlagType	HedgeFlag;
		///买卖方向
		TThostFtdcDirectionType	Direction;
		//组合拆分标记
		TThostFtdcCombDirectionType	CombDirection;
	};

	///组合拆分单最大量
	struct CKSMaxCombActionVolumeField
	{
		///经纪公司代码
		TThostFtdcBrokerIDType	BrokerID;
		///投资者代码
		TThostFtdcInvestorIDType	InvestorID;
		///最大可组合数量
		TThostFtdcVolumeType	MaxCombVolume;
		///最大可拆分数量
		TThostFtdcVolumeType	MaxActionVolume;
	};

	///查询行权指派明细
	struct CKSQryHistoryAssignmentField
	{
		///经纪公司代码
		TThostFtdcBrokerIDType	BrokerID;
		///投资者代码
		TThostFtdcInvestorIDType	InvestorID;
		///交易所代码
		TThostFtdcExchangeIDType	ExchangeID;
		///产品代码
		TThostFtdcInstrumentIDType	ProductID;
		///合约代码
		TThostFtdcInstrumentIDType	InstrumentID;
		///投保标记
		TThostFtdcHedgeFlagType	HedgeFlag;
		///期权类型
		TThostFtdcOptionsTypeType	OptionsType;
		///交割月
		TThostFtdcDateType	DeliveryMonth;
		///持仓方向
		TKSSOPosiDirectionType PosiDirection;
	};

	///查询行权交割明细
	struct CKSQrySODelivDetailField
	{
		///经纪公司代码
		TThostFtdcBrokerIDType	BrokerID;
		///投资者代码
		TThostFtdcInvestorIDType	InvestorID;
		///交易所代码
		TThostFtdcExchangeIDType	ExchangeID;
		///产品代码
		TThostFtdcInstrumentIDType	ProductID;
		///交割月
		TThostFtdcDateType	DeliveryMonth;
		///个股交收查询类型
		TKSSODelivModeType	DelivMode;
	};

	///自动行权执行操作
	struct CKSAutoExecOrderActionField
	{
		///经纪公司代码
		TThostFtdcBrokerIDType	BrokerID;
		///投资者代码
		TThostFtdcInvestorIDType	InvestorID;
		///自动行权阈值(0-不自动行权,非0-代表超过阀值会自动行权(20代表20%))
		TThostFtdcVolumeType RangeVol;
	};

	///行权指派明细
	struct CKSHistoryAssignmentField
	{
		///经纪公司代码
		TThostFtdcBrokerIDType	BrokerID;
		///投资者代码
		TThostFtdcInvestorIDType	InvestorID;
		///交易日期
		TThostFtdcTradeDateType	TradingDay;
		///产品代码
		TThostFtdcInstrumentIDType	ProductID;
		///合约代码
		TThostFtdcInstrumentIDType	InstrumentID;
		///客户代码
		TThostFtdcClientIDType	ClientID;
		///标的证券代码
		TThostFtdcInstrumentIDType	StockInstr;
		///投保标记
		TThostFtdcHedgeFlagType	HedgeFlag;
		///期权类型
		TThostFtdcOptionsTypeType	OptionsType;
		///持仓方向
		TKSSOPosiDirectionType PosiDirection;
		///执行价
		TThostFtdcPriceType	StrikePrice;
		///行权指派数量
		TThostFtdcVolumeType ExecVol;
		///标的证券应收付数量
		TThostFtdcVolumeType IOVol;
		///应收付金额
		TThostFtdcMoneyType	IOAmt;
		///行权交收日
		TThostFtdcDateType	DelivDate;
	};

	///行权交割明细
	struct CKSSODelivDetailField
	{
		///经纪公司代码
		TThostFtdcBrokerIDType	BrokerID;
		///投资者代码
		TThostFtdcInvestorIDType	InvestorID;
		///交易所代码
		TThostFtdcExchangeIDType	ExchangeID;
		///标的证券代码
		TThostFtdcInstrumentIDType	StockInstr;
		///应收/应付/扣券证券数量
		TThostFtdcVolumeType IOVol;
		///实收付数量
		TThostFtdcVolumeType IOVolInFact;
		///结算价
		TThostFtdcPriceType	SettlementPrice;
		///扣券面值/结算金额
		TThostFtdcMoneyType	SettlementAmt;
		///行权交收日
		TThostFtdcDateType	DelivDate;
		///业务类型名称
		TThostFtdcFunctionNameType	FunctionName;
	};

	///订制主数据业务
	struct CKSSubPrimeDataBusinessField
	{
		///经纪公司代码
		TThostFtdcBrokerIDType	BrokerID;
		///用户代码
		TThostFtdcUserIDType	UserID;
		///用户端产品信息
		TThostFtdcProductInfoType	UserProductInfo;
	};

	///主数据业务
	struct CKSPrimeDataBusinessField
	{
		///存储过程名称
		TThostFtdcProcessNameType ProcessName;
		///加密方式
		TThostFtdcFBTEncryModeType EncryMode;
		///密钥
		TThostFtdcPasswordKeyType PasswordKey;
		///数据归档状态
		TThostFtdcSaveStatusType SaveStatus;
	};

	///指定的合约
	struct CKSSpecificInstrumentField
	{
		///合约代码
		TThostFtdcInstrumentIDType	InstrumentID;
		///交易所代码
		TThostFtdcExchangeIDType	ExchangeID;
	};

	///查询合约保证金率
	struct CKSQryInstrumentMarginRateField
	{
		///经纪公司代码
		TThostFtdcBrokerIDType	BrokerID;
		///投资者代码
		TThostFtdcInvestorIDType	InvestorID;
		///合约代码
		TThostFtdcInstrumentIDType	InstrumentID;
		///投机套保标志
		TThostFtdcHedgeFlagType	HedgeFlag;
		///交易所代码
		TThostFtdcExchangeIDType	ExchangeID;
	};

	///合约保证金率
	struct CKSInstrumentMarginRateField
	{
		///合约代码
		TThostFtdcInstrumentIDType	InstrumentID;
		///投资者范围
		TThostFtdcInvestorRangeType	InvestorRange;
		///经纪公司代码
		TThostFtdcBrokerIDType	BrokerID;
		///投资者代码
		TThostFtdcInvestorIDType	InvestorID;
		///投机套保标志
		TThostFtdcHedgeFlagType	HedgeFlag;
		///多头保证金率
		TThostFtdcRatioType	LongMarginRatioByMoney;
		///多头保证金费
		TThostFtdcMoneyType	LongMarginRatioByVolume;
		///空头保证金率
		TThostFtdcRatioType	ShortMarginRatioByMoney;
		///空头保证金费
		TThostFtdcMoneyType	ShortMarginRatioByVolume;
		///是否相对交易所收取
		TThostFtdcBoolType	IsRelative;
		///交易所代码
		TThostFtdcExchangeIDType	ExchangeID;
	};

	///输入的个股组合拆分
	struct CKSInputCombActionField
	{
		///经纪公司代码
		TThostFtdcBrokerIDType	BrokerID;
		///投资者代码
		TThostFtdcInvestorIDType	InvestorID;
		///策略代码
		TKSCombStrategyIDType	StrategyID;
		///合约代码1
		TThostFtdcInstrumentIDType	InstrumentID1;
		///合约代码2
		TThostFtdcInstrumentIDType	InstrumentID2;
		///合约代码3
		TThostFtdcInstrumentIDType	InstrumentID3;
		///合约代码4
		TThostFtdcInstrumentIDType	InstrumentID4;
		///组合引用
		TThostFtdcOrderRefType	CombActionRef;
		///用户代码
		TThostFtdcUserIDType	UserID;
		///买卖方向1
		TThostFtdcDirectionType	Direction1;
		///买卖方向2
		TThostFtdcDirectionType	Direction2;
		///买卖方向3
		TThostFtdcDirectionType	Direction3;
		///买卖方向4
		TThostFtdcDirectionType	Direction4;
		///强拆标记
		TKSCombActionType  CombActionFlag;
		///数量
		TThostFtdcVolumeType	Volume;
		///组合拆分标记
		TThostFtdcCombDirectionType	CombDirection;
		///投机套保标志
		TThostFtdcHedgeFlagType	HedgeFlag;
		///交易所代码
		TThostFtdcExchangeIDType	ExchangeID;
		///交易所组合编号
		TThostFtdcTradeIDType	ComTradeID;
	};

	///个股组合拆分
	struct CKSCombActionField
	{
		///经纪公司代码
		TThostFtdcBrokerIDType	BrokerID;
		///投资者代码
		TThostFtdcInvestorIDType	InvestorID;
		///完整合约代码
		TThostFtdcInstrumentIDType	InstrumentID;
		///策略代码
		TKSCombStrategyIDType	StrategyID;
		///合约代码1
		TThostFtdcInstrumentIDType	InstrumentID1;
		///合约代码2
		TThostFtdcInstrumentIDType	InstrumentID2;
		///合约代码3
		TThostFtdcInstrumentIDType	InstrumentID3;
		///合约代码4
		TThostFtdcInstrumentIDType	InstrumentID4;
		///组合引用
		TThostFtdcOrderRefType	CombActionRef;
		///用户代码
		TThostFtdcUserIDType	UserID;
		///买卖方向1
		TThostFtdcDirectionType	Direction1;
		///买卖方向2
		TThostFtdcDirectionType	Direction2;
		///买卖方向3
		TThostFtdcDirectionType	Direction3;
		///买卖方向4
		TThostFtdcDirectionType	Direction4;
		///强拆标记
		TKSCombActionType  CombActionFlag;
		///数量
		TThostFtdcVolumeType	Volume;
		///组合拆分标记
		TThostFtdcCombDirectionType	CombDirection;
		///投机套保标志
		TThostFtdcHedgeFlagType	HedgeFlag;
		///本地申请组合编号
		TThostFtdcOrderLocalIDType	ActionLocalID;
		///交易所代码
		TThostFtdcExchangeIDType	ExchangeID;
		///会员代码
		TThostFtdcParticipantIDType	ParticipantID;
		///客户代码
		TThostFtdcClientIDType	ClientID;
		///合约在交易所的代码
		TThostFtdcExchangeInstIDType	ExchangeInstID;
		///交易所交易员代码
		TThostFtdcTraderIDType	TraderID;
		///安装编号
		TThostFtdcInstallIDType	InstallID;
		///组合状态
		TThostFtdcOrderActionStatusType	ActionStatus;
		///报单提示序号
		TThostFtdcSequenceNoType	NotifySequence;
		///交易日
		TThostFtdcDateType	TradingDay;
		///结算编号
		TThostFtdcSettlementIDType	SettlementID;
		///序号
		TThostFtdcSequenceNoType	SequenceNo;
		///前置编号
		TThostFtdcFrontIDType	FrontID;
		///会话编号
		TThostFtdcSessionIDType	SessionID;
		///用户端产品信息
		TThostFtdcProductInfoType	UserProductInfo;
		///状态信息
		TThostFtdcErrorMsgType	StatusMsg;
		///交易所组合编号
		TThostFtdcTradeIDType	ComTradeID;
		///报单来源
		TThostFtdcOrderSourceType	OrderSource;
	};

	///查询个股组合持仓明细
	struct CKSQryInvestorPositionCombineDetailField
	{
		///经纪公司代码
		TThostFtdcBrokerIDType	BrokerID;
		///投资者代码
		TThostFtdcInvestorIDType	InvestorID;
		///交易所代码
		TThostFtdcExchangeIDType	ExchangeID;
		///产品代码
		TThostFtdcInstrumentIDType	ProductID;
		///交易所组合编号
		TThostFtdcTradeIDType	ComTradeID;
	};

	///个股组合持仓明细
	struct CKSInvestorPositionCombineDetailField
	{
		///经纪公司代码
		TThostFtdcBrokerIDType	BrokerID;
		///投资者代码
		TThostFtdcInvestorIDType	InvestorID;
		///交易所代码
		TThostFtdcExchangeIDType	ExchangeID;
		///交易编码
		TThostFtdcClientIDType	ClientID;
		///组合编号
		TThostFtdcTradeIDType	ComTradeID;
		///策略代码
		TKSCombStrategyIDType	StrategyID;
		///合约代码1
		TThostFtdcInstrumentIDType	InstrumentID1;
		///合约代码2
		TThostFtdcInstrumentIDType	InstrumentID2;
		///合约代码3
		TThostFtdcInstrumentIDType	InstrumentID3;
		///合约代码4
		TThostFtdcInstrumentIDType	InstrumentID4;
		///占用保证金
		TThostFtdcMoneyType	Margin;
		///距离自动拆分日天数
		TThostFtdcVolumeType	CombActionVolume;
		///投保标记
		TThostFtdcHedgeFlagType	HedgeFlag;
		///持仓量
		TThostFtdcVolumeType	TotalAmt;
		///买卖方向1
		TThostFtdcDirectionType	Direction1;
		///买卖方向2
		TThostFtdcDirectionType	Direction2;
		///买卖方向3
		TThostFtdcDirectionType	Direction3;
		///买卖方向4
		TThostFtdcDirectionType	Direction4;
		///组合买卖方向
		TThostFtdcDirectionType	CombDirection;
	};

	///个股可组合可拆分手数查询请求
	struct CKSQryCombActionVolumeField
	{
		///经纪公司代码
		TThostFtdcBrokerIDType	BrokerID;
		///投资者代码
		TThostFtdcInvestorIDType	InvestorID;
		///交易所代码
		TThostFtdcExchangeIDType	ExchangeID;
		///策略代码
		TKSCombStrategyIDType	StrategyID;
		///合约代码1
		TThostFtdcInstrumentIDType	InstrumentID1;
		///合约代码2
		TThostFtdcInstrumentIDType	InstrumentID2;
		///合约代码3
		TThostFtdcInstrumentIDType	InstrumentID3;
		///合约代码4
		TThostFtdcInstrumentIDType	InstrumentID4;
		///投保标记
		TThostFtdcHedgeFlagType	HedgeFlag;
		///组合拆分标记
		TThostFtdcCombDirectionType	CombDirection;
		///买卖方向1
		TThostFtdcDirectionType	Direction1;
		///买卖方向2
		TThostFtdcDirectionType	Direction2;
		///买卖方向3
		TThostFtdcDirectionType	Direction3;
		///买卖方向4
		TThostFtdcDirectionType	Direction4;
		///强拆标记
		TKSCombActionType  CombActionFlag;
		///组合编号
		TThostFtdcTradeIDType	ComTradeID;
	};

	///个股可组合可拆分手数
	struct CKSCombActionVolumeField
	{
		///经纪公司代码
		TThostFtdcBrokerIDType	BrokerID;
		///投资者代码
		TThostFtdcInvestorIDType	InvestorID;
		///最大可组合数量
		TThostFtdcVolumeType	MaxCombVolume;
		///最大可拆分数量
		TThostFtdcVolumeType	MaxActionVolume;
	};

	///客户每日出金额度申请请求
	struct CKSInputFundOutCreditApplyField
	{
		///经纪公司代码
		TThostFtdcBrokerIDType	BrokerID;
		///投资者代码
		TThostFtdcInvestorIDType	InvestorID;
		///当日出金额度
		TThostFtdcMoneyType	FundOutCredit;
		///当日出金额度操作类型
		TKSFOCreditApplyType CreditApplyFlag;
		///流水号
		TThostFtdcTradeSerialNoType	CreditSerial;
	};

	///客户每日出金额度查询请求
	struct CKSQryFundOutCreditField
	{
		///经纪公司代码
		TThostFtdcBrokerIDType	BrokerID;
		///投资者代码
		TThostFtdcInvestorIDType	InvestorID;
	};

	///客户每日出金额度查询
	struct CKSRspQryFundOutCreditField
	{
		///经纪公司代码
		TThostFtdcBrokerIDType	BrokerID;
		///投资者代码
		TThostFtdcInvestorIDType	InvestorID;
		///当日出金额度
		TThostFtdcMoneyType	FundOutCredit;
		///当日已出金额度
		TThostFtdcMoneyType	FundOutedCredit;
		///最后更新日期
		TThostFtdcDateType	UpdateDate;
		///设置日期
		TThostFtdcDateType	SetDate;
		///设置时间
		TThostFtdcTimeType	SetTime;
	};

	///客户每日出金额度申请查询请求
	struct CKSQryFundOutCreditApplyField
	{
		///经纪公司代码
		TThostFtdcBrokerIDType	BrokerID;
		///投资者代码
		TThostFtdcInvestorIDType	InvestorID;
		///查询开始日期
		TThostFtdcDateType	QryCreditStart;
		///查询结束日期
		TThostFtdcDateType	QryCreditEnd;
		///当日出金额度处理状态
		TKSFOCreditStatusType	DealStatus;
	};

	///客户每日出金额度申请查询
	struct CKSRspQryFundOutCreditApplyField
	{
		///经纪公司代码
		TThostFtdcBrokerIDType	BrokerID;
		///投资者代码
		TThostFtdcInvestorIDType	InvestorID;
		///流水号
		TThostFtdcTradeSerialNoType	CreditSerial;
		///申请出金额度
		TThostFtdcMoneyType	FundOutCredit;
		///当日出金额度处理状态
		TKSFOCreditStatusType	DealStatus;
		///申请日期
		TThostFtdcDateType	ApplyDate;
		///申请时间
		TThostFtdcTimeType	ApplyTime;
		///审核日期
		TThostFtdcDateType	ConfirmDate;
		///审核时间
		TThostFtdcTimeType	ConfirmTime;
	};

	///大额出金预约（取消）申请
	struct CKSLargeFundOutApplyField
	{
		///经纪公司代码
		TThostFtdcBrokerIDType	BrokerID;
		///投资者代码
		TThostFtdcInvestorIDType	InvestorID;
		///预约出金日期
		TThostFtdcDateType	ApplyDate;
		///操作类型
		TKSFOCreditApplyType ApplyFlag;
		///流水号
		TThostFtdcTradeSerialNoType	ApplySerial;
		///预约出金金额
		TThostFtdcMoneyType	FundOutCredit;
		///币种代码
		TThostFtdcCurrencyIDType	CurrencyID;
		///银行代码
		TThostFtdcBankIDType	BankID;		
	};

	///大额出金预约查询请求
	struct CKSQryLargeFundOutApplyField
	{
		///经纪公司代码
		TThostFtdcBrokerIDType	BrokerID;
		///投资者代码
		TThostFtdcInvestorIDType	InvestorID;
		///预约申请日期
		TThostFtdcDateType	ApplyDate;
		///预约出金日期
		TThostFtdcDateType	FundOutDate;
		///审核状态
		TKSFOCreditStatusType ApplyFlag;
		///银行代码
		TThostFtdcBankIDType	BankID;		
	};

	///大额出金预约查询
	struct CKSRspLargeFundOutApplyField
	{
		///经纪公司代码
		TThostFtdcBrokerIDType	BrokerID;
		///投资者代码
		TThostFtdcInvestorIDType	InvestorID;
		///流水号
		TThostFtdcTradeSerialNoType	ApplySerial;
		///审核状态
		TKSFOCreditStatusType ApplyFlag;
		///银行代码
		TThostFtdcBankIDType	BankID;	
		///币种代码
		TThostFtdcCurrencyIDType	CurrencyID;
		///预约出金金额
		TThostFtdcMoneyType	FundOutCredit;	
		///预约申请日期
		TThostFtdcDateType	ApplyDate;
		///预约出金日期
		TThostFtdcDateType	FundOutDate;
		///设置日期
		TThostFtdcDateType	SetDate;
		///设置时间
		TThostFtdcTimeType	SetTime;
	};

	///期权手续费率查询
	struct CKSQryOptionInstrCommRateField
	{
		///经纪公司代码
		TThostFtdcBrokerIDType	BrokerID;
		///投资者代码
		TThostFtdcInvestorIDType	InvestorID;
		///合约代码
		TThostFtdcInstrumentIDType	InstrumentID;
		///交易所代码
		TThostFtdcExchangeIDType	ExchangeID;
	};

	///当前期权合约手续费的详细内容
	struct CKSOptionInstrCommRateField
	{
		///合约代码
		TThostFtdcInstrumentIDType	InstrumentID;
		///投资者范围
		TThostFtdcInvestorRangeType	InvestorRange;
		///经纪公司代码
		TThostFtdcBrokerIDType	BrokerID;
		///投资者代码
		TThostFtdcInvestorIDType	InvestorID;
		///买开仓手续费率
		TThostFtdcRatioType	BuyOpenRatioByMoney;
		///买开仓手续费
		TThostFtdcRatioType	BuyOpenRatioByVolume;
		///买平仓手续费率
		TThostFtdcRatioType	BuyCloseRatioByMoney;
		///买平仓手续费
		TThostFtdcRatioType	BuyCloseRatioByVolume;
		///行权手续费率
		TThostFtdcRatioType	StrikeRatioByMoney;
		///行权手续费
		TThostFtdcRatioType	StrikeRatioByVolume;
		///卖开仓手续费率
		TThostFtdcRatioType	SellOpenRatioByMoney;
		///卖开仓手续费
		TThostFtdcRatioType	SellOpenRatioByVolume;
		///卖平仓手续费率
		TThostFtdcRatioType	SellCloseRatioByMoney;
		///卖平仓手续费
		TThostFtdcRatioType	SellCloseRatioByVolume;
		///备兑开仓手续费率
		TThostFtdcRatioType	CoveredOpenRatioByMoney;
		///备兑开仓手续费
		TThostFtdcRatioType	CoveredOpenRatioByVolume;
		///备兑平仓手续费率
		TThostFtdcRatioType	CoveredCloseRatioByMoney;
		///备兑平仓手续费
		TThostFtdcRatioType	CoveredCloseRatioByVolume;
		///交易所代码
		TThostFtdcExchangeIDType	ExchangeID;
	};
}	// end of namespace KingstarAPI
#endif