#include "stdafx.h"
#include "TypeConvert.h"


/// 类似于OpenQuant FIX一样的效果，插件层简单，基本不要做怎么计算或处理
/// 对于一个单子的某个状态可能是这样的，新单，部分成交，完全成交
/// EmitAccept,EmitFill
/// OnRtnOrder,OnRtnTrade,如何转成Emit
/// EmitAccept是什么
///
/// 接口向外提供的回报可以分两种方案，ExecutionReport或委托回报与成交回报
/// OpenQuant中使用ExecutionReport问题是因为OQ自己有OrderManager，如果其它软件要看到委托和成交列表是没法得到的
/// 所以接口应当返回委托与成交回报

PutCall TUstpFtdcOptionsTypeType_2_PutCall(TUstpFtdcOptionsTypeType In)
{
	if (In == USTP_FTDC_OT_CallOptions)
		return PutCall::Call;
	return PutCall::Put;
}

HedgeFlagType TUstpFtdcHedgeFlagType_2_HedgeFlagType(TUstpFtdcHedgeFlagType In)
{
	switch (In)
	{
	case USTP_FTDC_CHF_Arbitrage:
		return HedgeFlagType::Arbitrage;
	case USTP_FTDC_CHF_Hedge:
		return HedgeFlagType::Hedge;
	case USTP_FTDC_CHF_MarketMaker:
		return HedgeFlagType::MarketMaker;
	case USTP_FTDC_CHF_Speculation:
	default:
		return HedgeFlagType::Speculation;
	}
}

TUstpFtdcHedgeFlagType HedgeFlagType_2_TUstpFtdcHedgeFlagType(HedgeFlagType In)
{
	switch (In)
	{
	case HedgeFlagType::Arbitrage:
		return USTP_FTDC_CHF_Arbitrage;
	case HedgeFlagType::Hedge:
		return USTP_FTDC_CHF_Hedge;
	case HedgeFlagType::MarketMaker:
		return USTP_FTDC_CHF_MarketMaker;
	case HedgeFlagType::Speculation:
	default:
		return USTP_FTDC_CHF_Speculation;
	}
}

OpenCloseType TUstpFtdcOffsetFlagType_2_OpenCloseType(TUstpFtdcOffsetFlagType In)
{
	switch (In)
	{
	case USTP_FTDC_OF_CloseToday:
		return OpenCloseType::CloseToday;
	case USTP_FTDC_OF_Close:
		return OpenCloseType::Close;
	case USTP_FTDC_OF_Open:
	default:
		return OpenCloseType::Open;
	}
}

TUstpFtdcOffsetFlagType OpenCloseType_2_TUstpFtdcOffsetFlagType(OpenCloseType In)
{
	switch (In)
	{
	case OpenCloseType::CloseToday:
		return USTP_FTDC_OF_CloseToday;
	case OpenCloseType::Close:
		return USTP_FTDC_OF_Close;
	case OpenCloseType::Open:
	default:
		return USTP_FTDC_OF_Open;
	}
}

TUstpFtdcDirectionType OrderSide_2_TUstpFtdcDirectionType(OrderSide In)
{
	if (In == OrderSide::Sell)
		return USTP_FTDC_D_Sell;
	return USTP_FTDC_D_Buy;
}

OrderSide TUstpFtdcDirectionType_2_OrderSide(TUstpFtdcDirectionType In)
{
	if (In == USTP_FTDC_D_Sell)
		return OrderSide::Sell;
	return OrderSide::Buy;
}

PositionSide TUstpFtdcDirectionType_2_PositionSide(TUstpFtdcDirectionType In)
{
	if (In == USTP_FTDC_D_Sell)
		return PositionSide::Short;
	return PositionSide::Long;
}

TUstpFtdcOrderPriceTypeType OrderType_2_TUstpFtdcOrderPriceTypeType(OrderType In)
{
	switch (In)
	{
	case Market:
		return USTP_FTDC_OPT_AnyPrice;
	case Stop:
		return USTP_FTDC_OPT_AnyPrice;
	case Limit:
		return USTP_FTDC_OPT_LimitPrice;
	case StopLimit:
		return USTP_FTDC_OPT_LimitPrice;
	case MarketOnClose:
		return USTP_FTDC_OPT_AnyPrice;
	case TrailingStop:
		return USTP_FTDC_OPT_AnyPrice;
	case TrailingStopLimit:
		return USTP_FTDC_OPT_LimitPrice;
	default:
		return USTP_FTDC_OPT_LimitPrice;
	}
}

OrderType CUstpFtdcOrderField_2_OrderType(CUstpFtdcOrderField* pIn)
{
	switch (pIn->OrderPriceType)
	{
	case USTP_FTDC_OPT_AnyPrice:
		return OrderType::Market;
	case USTP_FTDC_OPT_LimitPrice:
		return OrderType::Limit;
	default:
		return OrderType::Limit;
	}
}

TimeInForce CUstpFtdcOrderField_2_TimeInForce(CUstpFtdcOrderField* pIn)
{
	switch (pIn->TimeCondition)
	{
	case USTP_FTDC_TC_GFD:
		return TimeInForce::Day;
	case USTP_FTDC_TC_IOC:
		switch (pIn->VolumeCondition)
		{
		case USTP_FTDC_VC_AV:
			return TimeInForce::IOC;
		case USTP_FTDC_VC_CV:
			return TimeInForce::FOK;
		default:
			return TimeInForce::IOC;
		}
	default:
		return TimeInForce::Day;
	}
}

OrderStatus CUstpFtdcOrderField_2_OrderStatus(CUstpFtdcOrderField* pIn)
{
	switch (pIn->OrderStatus)
	{
	case USTP_FTDC_OS_Canceled:
		return OrderStatus::Cancelled;
	case USTP_FTDC_OS_NoTradeQueueing:
		return OrderStatus::New;
	default:
		if (pIn->VolumeRemain == 0)
			return OrderStatus::Filled;
		else if (pIn->VolumeRemain == pIn->Volume)
			return OrderStatus::New;
		else
			return OrderStatus::PartiallyFilled;
	}
}

ExecType CUstpFtdcOrderField_2_ExecType(CUstpFtdcOrderField* pIn)
{
	switch (pIn->OrderStatus)
	{
	case USTP_FTDC_OS_Canceled:
		return ExecType::ExecCancelled;
	case USTP_FTDC_OS_NoTradeQueueing:
		return ExecType::ExecNew;
	case USTP_FTDC_OS_AllTraded:
	case USTP_FTDC_OS_PartTradedQueueing:
		return ExecType::ExecTrade;
	default:
		return ExecType::ExecNew;
	}
}

OrderStatus CUstpFtdcRtnQuoteField_2_OrderStatus(CUstpFtdcRtnQuoteField* pIn)
{
	switch (pIn->QuoteStatus)
	{
	case USTP_FTDC_QS_Inited_InFEMAS:
	case USTP_FTDC_QS_Accepted_InTradingSystem:
		return OrderStatus::New;
	case USTP_FTDC_QS_Canceled_SingleLeg:
	case USTP_FTDC_QS_Canceled_All:
		return OrderStatus::Cancelled;
	case USTP_FTDC_QS_Traded_SingleLeg:
		return OrderStatus::PartiallyFilled;
	case USTP_FTDC_QS_Traded_All:
		return OrderStatus::Filled;
	case USTP_FTDC_QS_Error_QuoteAction:
		return OrderStatus::PendingCancel;
	default:
		return OrderStatus::New;
	}
}

ExecType CUstpFtdcRtnQuoteField_2_ExecType(CUstpFtdcRtnQuoteField* pIn)
{
	switch (pIn->QuoteStatus)
	{
	case USTP_FTDC_QS_Inited_InFEMAS:
	case USTP_FTDC_QS_Accepted_InTradingSystem:
		return ExecType::ExecNew;
	case USTP_FTDC_QS_Canceled_SingleLeg:
	case USTP_FTDC_QS_Canceled_All:
		return ExecType::ExecCancelled;
	case USTP_FTDC_QS_Traded_SingleLeg:
	case USTP_FTDC_QS_Traded_All:
		return ExecType::ExecTrade;
	case USTP_FTDC_QS_Error_QuoteAction:
		return ExecType::ExecCancelReject;
	default:
		return ExecType::ExecNew;
	}
}

InstrumentType CUstpFtdcRspInstrumentField_2_InstrumentType(CUstpFtdcRspInstrumentField* pIn)
{
	switch (pIn->OptionsType)
	{
	case USTP_FTDC_OT_NotOptions:
		return InstrumentType::Future;
	default:
		return InstrumentType::Option;
	}
}
