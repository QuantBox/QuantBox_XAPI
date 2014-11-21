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

PutCall TThostFtdcOptionsTypeType_2_PutCall(TThostFtdcOptionsTypeType In)
{
	if (In == THOST_FTDC_CP_CallOptions)
		return PutCall::Call;
	return PutCall::Put;
}

HedgeFlagType TThostFtdcHedgeFlagType_2_HedgeFlagType(TThostFtdcHedgeFlagType In)
{
	switch (In)
	{
	case THOST_FTDC_HF_Arbitrage:
		return HedgeFlagType::Arbitrage;
	case THOST_FTDC_HF_Hedge:
		return HedgeFlagType::Hedge;
	case THOST_FTDC_HF_Speculation:
	default:
		return HedgeFlagType::Speculation;
	}
}

TThostFtdcHedgeFlagType HedgeFlagType_2_TThostFtdcHedgeFlagType(HedgeFlagType In)
{
	switch (In)
	{
	case HedgeFlagType::Arbitrage:
		return THOST_FTDC_HF_Arbitrage;
	case HedgeFlagType::Hedge:
		return THOST_FTDC_HF_Hedge;
	case HedgeFlagType::Speculation:
	default:
		return THOST_FTDC_HF_Speculation;
	}
}

OpenCloseType TThostFtdcOffsetFlagType_2_OpenCloseType(TThostFtdcOffsetFlagType In)
{
	switch (In)
	{
	case THOST_FTDC_OF_CloseToday:
		return OpenCloseType::CloseToday;
	case THOST_FTDC_OF_Close:
		return OpenCloseType::Close;
	case THOST_FTDC_OF_Open:
	default:
		return OpenCloseType::Open;
	}
}

TThostFtdcOffsetFlagType OpenCloseType_2_TThostFtdcOffsetFlagType(OpenCloseType In)
{
	switch (In)
	{
	case OpenCloseType::CloseToday:
		return THOST_FTDC_OF_CloseToday;
	case OpenCloseType::Close:
		return THOST_FTDC_OF_Close;
	case OpenCloseType::Open:
	default:
		return THOST_FTDC_OF_Open;
	}
}

TThostFtdcDirectionType OrderSide_2_TThostFtdcDirectionType(OrderSide In)
{
	if (In == OrderSide::Sell)
		return THOST_FTDC_D_Sell;
	return THOST_FTDC_D_Buy;
}

OrderSide TThostFtdcDirectionType_2_OrderSide(TThostFtdcDirectionType In)
{
	if (In == THOST_FTDC_D_Sell)
		return OrderSide::Sell;
	return OrderSide::Buy;
}

PositionSide TThostFtdcPosiDirectionType_2_PositionSide(TThostFtdcPosiDirectionType In)
{
	if (In == THOST_FTDC_PD_Short)
		return PositionSide::Short;
	return PositionSide::Long;
}

PositionSide TradeField_2_PositionSide(TradeField* pIn)
{
	if (pIn->OpenClose == OpenCloseType::Open)
	{
		if (pIn->Side == OrderSide::Buy)
			return PositionSide::Long;
		return PositionSide::Short;
	}
	else
	{
		if (pIn->Side == OrderSide::Buy)
			return PositionSide::Short;
		return PositionSide::Long;
	}
}

TThostFtdcOrderPriceTypeType OrderType_2_TThostFtdcOrderPriceTypeType(OrderType In)
{
	switch (In)
	{
	case Market:
		return THOST_FTDC_OPT_AnyPrice;
	case Stop:
		return THOST_FTDC_OPT_AnyPrice;
	case Limit:
		return THOST_FTDC_OPT_LimitPrice;
	case StopLimit:
		return THOST_FTDC_OPT_LimitPrice;
	case MarketOnClose:
		return THOST_FTDC_OPT_AnyPrice;
	case TrailingStop:
		return THOST_FTDC_OPT_AnyPrice;
	case TrailingStopLimit:
		return THOST_FTDC_OPT_LimitPrice;
	default:
		return THOST_FTDC_OPT_LimitPrice;
	}
}

OrderStatus CThostFtdcOrderField_2_OrderStatus(CThostFtdcOrderField* pIn)
{
	switch (pIn->OrderStatus)
	{
	case THOST_FTDC_OST_Canceled:
		if (pIn->OrderSubmitStatus == THOST_FTDC_OSS_InsertRejected)
			return OrderStatus::Rejected;
		return OrderStatus::Cancelled;
	case THOST_FTDC_OST_Unknown:
		// 如果是撤单，也有可能出现这一条，如何过滤？
		if (pIn->OrderSubmitStatus == THOST_FTDC_OSS_InsertSubmitted)
			return OrderStatus::New;
	default:
		if (pIn->VolumeTotal == 0)
			return OrderStatus::Filled;
		else if (pIn->VolumeTotal == pIn->VolumeTotalOriginal)
			return OrderStatus::New;
		else
			return OrderStatus::PartiallyFilled;
	}
}

ExecType CThostFtdcOrderField_2_ExecType(CThostFtdcOrderField* pIn)
{
	switch (pIn->OrderStatus)
	{
	case THOST_FTDC_OST_Canceled:
		if (pIn->OrderSubmitStatus == THOST_FTDC_OSS_InsertRejected)
			return ExecType::ExecRejected;
		return ExecType::ExecCancelled;
	case THOST_FTDC_OST_Unknown:
		// 如果是撤单，也有可能出现这一条，如何过滤？
		if (pIn->OrderSubmitStatus == THOST_FTDC_OSS_InsertSubmitted)
			return ExecType::ExecNew;
	case THOST_FTDC_OST_AllTraded:
	case THOST_FTDC_OST_PartTradedQueueing:
		return ExecType::ExecTrade;
	default:
		return ExecType::ExecNew;
	}
}

OrderStatus CThostFtdcQuoteField_2_OrderStatus(CThostFtdcQuoteField* pIn)
{
	switch (pIn->QuoteStatus)
	{
	case THOST_FTDC_OST_Canceled:
		if (pIn->OrderSubmitStatus == THOST_FTDC_OSS_InsertRejected)
			return OrderStatus::Rejected;
		return OrderStatus::Cancelled;
	case THOST_FTDC_OST_Unknown:
		// 如果是撤单，也有可能出现这一条，如何过滤？
		if (pIn->OrderSubmitStatus == THOST_FTDC_OSS_InsertSubmitted)
			return OrderStatus::New;
	case THOST_FTDC_OST_Touched:
		if (pIn->OrderSubmitStatus == THOST_FTDC_OSS_InsertRejected)
			return OrderStatus::Rejected;
	default:
		// 这个地方要改

		//if (pIn->VolumeTotal == 0)
		//	return OrderStatus::Filled;
		//else if (pIn->VolumeTotal == pIn->VolumeTotalOriginal)
			return OrderStatus::New;
		//else
		//	return OrderStatus::PartiallyFilled;
	}
}

ExecType CThostFtdcQuoteField_2_ExecType(CThostFtdcQuoteField* pIn)
{
	switch (pIn->QuoteStatus)
	{
	case THOST_FTDC_OST_Canceled:
		if (pIn->OrderSubmitStatus == THOST_FTDC_OSS_InsertRejected)
			return ExecType::ExecRejected;
		return ExecType::ExecCancelled;
	case THOST_FTDC_OST_Unknown:
		// 如果是撤单，也有可能出现这一条，如何过滤？
		if (pIn->OrderSubmitStatus == THOST_FTDC_OSS_InsertSubmitted)
			return ExecType::ExecNew;
	case THOST_FTDC_OST_AllTraded:
	case THOST_FTDC_OST_PartTradedQueueing:
		return ExecType::ExecTrade;
	case THOST_FTDC_OST_Touched:
		if (pIn->OrderSubmitStatus == THOST_FTDC_OSS_InsertRejected)
			return ExecType::ExecRejected;
	default:
		return ExecType::ExecNew;
	}
}



OrderType CThostFtdcOrderField_2_OrderType(CThostFtdcOrderField* pIn)
{
	switch (pIn->OrderPriceType)
	{
	case THOST_FTDC_OPT_AnyPrice:
		return OrderType::Market;
	case THOST_FTDC_OPT_LimitPrice:
		return OrderType::Limit;
	default:
		return OrderType::Limit;
	}
}

TimeInForce CThostFtdcOrderField_2_TimeInForce(CThostFtdcOrderField* pIn)
{
	switch (pIn->TimeCondition)
	{
	case THOST_FTDC_TC_GFD:
		return TimeInForce::Day;
	case THOST_FTDC_TC_IOC:
		switch (pIn->VolumeCondition)
		{
		case THOST_FTDC_VC_AV:
			return TimeInForce::IOC;
		case THOST_FTDC_VC_CV:
			return TimeInForce::FOK;
		default:
			return TimeInForce::IOC;
		}
	default:
		return TimeInForce::Day;
	}
}





InstrumentType CThostFtdcInstrumentField_2_InstrumentType(CThostFtdcInstrumentField* pIn)
{
	switch (pIn->ProductClass)
	{
	case THOST_FTDC_PC_Futures:
		return InstrumentType::Future;
	case THOST_FTDC_PC_Options:
		return InstrumentType::Option;
	case THOST_FTDC_PC_Combination:
		return InstrumentType::MultiLeg;
	case THOST_FTDC_PC_EFP:
		return InstrumentType::Future;
	case THOST_FTDC_PC_SpotOption:
		return InstrumentType::Option;
	default:
		return InstrumentType::Stock;
	}
}
