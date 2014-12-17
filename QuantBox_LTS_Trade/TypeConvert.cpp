#include "stdafx.h"
#include "TypeConvert.h"

#include <string.h>
/// 类似于OpenQuant FIX一样的效果，插件层简单，基本不要做怎么计算或处理
/// 对于一个单子的某个状态可能是这样的，新单，部分成交，完全成交
/// EmitAccept,EmitFill
/// OnRtnOrder,OnRtnTrade,如何转成Emit
/// EmitAccept是什么
///
/// 接口向外提供的回报可以分两种方案，ExecutionReport或委托回报与成交回报
/// OpenQuant中使用ExecutionReport问题是因为OQ自己有OrderManager，如果其它软件要看到委托和成交列表是没法得到的
/// 所以接口应当返回委托与成交回报


HedgeFlagType TSecurityFtdcHedgeFlagType_2_HedgeFlagType(TSecurityFtdcHedgeFlagType In)
{
	switch (In)
	{
	//case SECURITY_FTDC_HF_Arbitrage:
	//	return HedgeFlagType::Arbitrage;
	case SECURITY_FTDC_HF_Hedge:
		return HedgeFlagType::Hedge;
	case SECURITY_FTDC_HF_Speculation:
	default:
		return HedgeFlagType::Speculation;
	}
}

TSecurityFtdcHedgeFlagType HedgeFlagType_2_TSecurityFtdcHedgeFlagType(HedgeFlagType In)
{
	switch (In)
	{
	//case HedgeFlagType::Arbitrage:
	//	return SECURITY_FTDC_HF_Arbitrage;
	case HedgeFlagType::Hedge:
		return SECURITY_FTDC_HF_Hedge;
	case HedgeFlagType::Speculation:
	default:
		return SECURITY_FTDC_HF_Speculation;
	}
}

OpenCloseType TSecurityFtdcOffsetFlagType_2_OpenCloseType(TSecurityFtdcOffsetFlagType In)
{
	switch (In)
	{
	case SECURITY_FTDC_OF_CloseToday:
		return OpenCloseType::CloseToday;
	case SECURITY_FTDC_OF_Close:
		return OpenCloseType::Close;
	case SECURITY_FTDC_OF_Open:
	default:
		return OpenCloseType::Open;
	}
}

TSecurityFtdcOffsetFlagType OpenCloseType_2_TSecurityFtdcOffsetFlagType(OpenCloseType In)
{
	switch (In)
	{
	case OpenCloseType::CloseToday:
		return SECURITY_FTDC_OF_CloseToday;
	case OpenCloseType::Close:
		return SECURITY_FTDC_OF_Close;
	case OpenCloseType::Open:
	default:
		return SECURITY_FTDC_OF_Open;
	}
}

TSecurityFtdcDirectionType OrderSide_2_TSecurityFtdcDirectionType(OrderSide In)
{
	if (In == OrderSide::Sell)
		return SECURITY_FTDC_D_Sell;
	return SECURITY_FTDC_D_Buy;
}

OrderSide TSecurityFtdcDirectionType_2_OrderSide(TSecurityFtdcDirectionType In)
{
	if (In == SECURITY_FTDC_D_Sell)
		return OrderSide::Sell;
	return OrderSide::Buy;
}

PositionSide TSecurityFtdcPosiDirectionType_2_PositionSide(TSecurityFtdcPosiDirectionType In)
{
	if (In == SECURITY_FTDC_PD_Short)
		return PositionSide::Short;
	return PositionSide::Long;
}

PositionSide TradeField_2_PositionSide(TradeField* pIn)
{
	//if (pIn->OpenClose == OpenCloseType::Open)
	//{
	//	if (pIn->Side == OrderSide::Buy)
	//		return PositionSide::Long;
	//	return PositionSide::Short;
	//}
	//else
	//{
	//	if (pIn->Side == OrderSide::Buy)
	//		return PositionSide::Short;
	//	return PositionSide::Long;
	//}
	return PositionSide::Long;
}

TSecurityFtdcOrderPriceTypeType OrderType_2_TSecurityFtdcOrderPriceTypeType(OrderType In)
{
	switch (In)
	{
	case Market:
		return SECURITY_FTDC_OPT_AnyPrice;
	case Stop:
		return SECURITY_FTDC_OPT_AnyPrice;
	case Limit:
		return SECURITY_FTDC_OPT_LimitPrice;
	case StopLimit:
		return SECURITY_FTDC_OPT_LimitPrice;
	case MarketOnClose:
		return SECURITY_FTDC_OPT_AnyPrice;
	case TrailingStop:
		return SECURITY_FTDC_OPT_AnyPrice;
	case TrailingStopLimit:
		return SECURITY_FTDC_OPT_LimitPrice;
	default:
		return SECURITY_FTDC_OPT_LimitPrice;
	}
}

OrderStatus CSecurityFtdcOrderField_2_OrderStatus(CSecurityFtdcOrderField* pIn)
{
	switch (pIn->OrderStatus)
	{
	case SECURITY_FTDC_OST_Canceled:
		if (pIn->OrderSubmitStatus == SECURITY_FTDC_OSS_InsertRejected)
			return OrderStatus::Rejected;
		return OrderStatus::Cancelled;
	case SECURITY_FTDC_OST_Unknown:
		// 如果是撤单，也有可能出现这一条，如何过滤？
		if (pIn->OrderSubmitStatus == SECURITY_FTDC_OSS_InsertSubmitted)
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

OrderType CSecurityFtdcOrderField_2_OrderType(CSecurityFtdcOrderField* pIn)
{
	switch (pIn->OrderPriceType)
	{
	case SECURITY_FTDC_OPT_AnyPrice:
		return OrderType::Market;
	case SECURITY_FTDC_OPT_LimitPrice:
		return OrderType::Limit;
	default:
		return OrderType::Limit;
	}
}

TimeInForce CSecurityFtdcOrderField_2_TimeInForce(CSecurityFtdcOrderField* pIn)
{
	switch (pIn->TimeCondition)
	{
	case SECURITY_FTDC_TC_GFD:
		return TimeInForce::Day;
	case SECURITY_FTDC_TC_IOC:
		switch (pIn->VolumeCondition)
		{
		case SECURITY_FTDC_VC_AV:
			return TimeInForce::IOC;
		case SECURITY_FTDC_VC_CV:
			return TimeInForce::FOK;
		default:
			return TimeInForce::IOC;
		}
	default:
		return TimeInForce::Day;
	}
}

ExecType CSecurityFtdcOrderField_2_ExecType(CSecurityFtdcOrderField* pIn)
{
	switch (pIn->OrderStatus)
	{
	case SECURITY_FTDC_OST_Canceled:
		if (pIn->OrderSubmitStatus == SECURITY_FTDC_OSS_InsertRejected)
			return ExecType::ExecRejected;
		return ExecType::ExecCancelled;
	case SECURITY_FTDC_OST_Unknown:
		// 如果是撤单，也有可能出现这一条，如何过滤？
		if (pIn->OrderSubmitStatus == SECURITY_FTDC_OSS_InsertSubmitted)
			return ExecType::ExecNew;
	case SECURITY_FTDC_OST_AllTraded:
	case SECURITY_FTDC_OST_PartTradedQueueing:
		return ExecType::ExecTrade;
	default:
		return ExecType::ExecNew;
	}
}

InstrumentType CSecurityFtdcInstrumentField_2_InstrumentType(CSecurityFtdcInstrumentField* pIn)
{
	switch (pIn->ProductClass)
	{
	case SECURITY_FTDC_PC_Futures:
		return InstrumentType::Future;
	case SECURITY_FTDC_PC_Options:
		return InstrumentType::Option;
	case SECURITY_FTDC_PC_Combination:
		return InstrumentType::MultiLeg;
	case SECURITY_FTDC_PC_EFP:
		return InstrumentType::Future;
	//case SECURITY_FTDC_PC_SpotOption:
	//	return InstrumentType::Option;
	default:
		if (strlen(pIn->InstrumentID) == 8)
		{
			return InstrumentType::Option;
		}
		return InstrumentType::Stock;
	}
}

PutCall CSecurityFtdcInstrumentField_2_PutCall(CSecurityFtdcInstrumentField* pIn)
{
	if (strlen(pIn->InstrumentID) == 8)
	{
		if (pIn->ExchangeInstID[6] == 'C')
		{
			return PutCall::Call;
		}
	}
	return PutCall::Put;
}

PriceType CSecurityFtdcInstrumentField_2_PriceTick(CSecurityFtdcInstrumentField* pIn)
{
	if (pIn->PriceTick != 0)
		return pIn->PriceTick;

	// 期权为0.001
	if (strlen(pIn->InstrumentID) == 8)
	{
		return 0.001;
	}

	return 0.01;
}
