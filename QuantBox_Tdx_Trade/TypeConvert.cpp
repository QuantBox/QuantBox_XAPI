#include "stdafx.h"
#include "TypeConvert.h"
#include "../include/Tdx/tdx_enum.h"

// 将中文的报价方式转成委托方式，这是根据字符串的特点进行分类
int BJFS_2_WTFS(char* pIn)
{
	char* pX1 = strstr(pIn,"限");
	if (pX1 == pIn)
	{
		// 第一个字是限价
		return WTFS_Limit;
	}
	else
	{
		char* pC = strstr(pIn, "撤");
		if (pC)
		{
			char* p5 = strstr(pIn, "五");
			if (p5)
			{
				return WTFS_Five_IOC;
			}
			char* pQ = strstr(pIn, "全");
			if (pQ)
			{
				return WTFS_FOK;
			}
			return WTFS_IOC; // 剩
		}
		else
		{
			char* pZ = strstr(pIn, "转");
			if (pZ)
			{
				return WTFS_Five_Limit;
			}
			char* pD = strstr(pIn, "对");
			if (pD)
			{
				return WTFS_Best_Reverse;
			}
			return WTFS_Best_Forward; // 本
		}
	}
}

OrderType WTFS_2_OrderType(int In)
{
	switch (In)
	{
	case WTFS_Limit:
		return OrderType::Limit;
	default:
		return OrderType::Market;
	}
}

TimeInForce WTFS_2_TimeInForce(int In)
{
	switch (In)
	{
	case WTFS_Limit:
	case WTFS_Best_Reverse:
	case WTFS_Best_Forward:
		return TimeInForce::Day;
	case WTFS_IOC:
	case WTFS_Five_IOC:
		return TimeInForce::IOC;
	case WTFS_FOK:
		return TimeInForce::FOK;
	case WTFS_Five_Limit:
		return TimeInForce::Day;
	default:
		return TimeInForce::Day;
	}
}

OrderStatus ZTSM_2_OrderStatus(int In)
{
	switch (In)
	{
	case ZTSM_0:
		return OrderStatus::PendingNew;
	case ZTSM_New:
		return OrderStatus::New;
	case ZTSM_Illegal:
		return OrderStatus::Rejected;
	case ZTSM_AllFilled:
		return OrderStatus::Filled;
	case ZTSM_AllCancelled:
		return OrderStatus::Cancelled;
	default:
		return OrderStatus::NotSent;
	}
}

ExecType ZTSM_2_ExecType(int In)
{
	switch (In)
	{
	case ZTSM_0:
		return ExecType::ExecNew;
	case ZTSM_New:
		return ExecType::ExecNew;
	case ZTSM_Illegal:
		return ExecType::ExecRejected;
	case ZTSM_AllFilled:
		return ExecType::ExecTrade;
	case ZTSM_AllCancelled:
		return ExecType::ExecCancelled;
	default:
		return ExecType::ExecNew;
	}
}

bool ZTSM_IsDone(int In)
{
	switch (In)
	{
	case ZTSM_Illegal:
	case ZTSM_AllFilled:
	case ZTSM_AllCancelled:
		return true;
	}
	return false;
}

bool ZTSM_IsNotSent(int In)
{
	switch (In)
	{
	case ZTSM_0:
		return true;
	}
	return false;
}

// 将买卖方式转成买卖方向
OrderSide MMBZ_2_OrderSide(int In)
{
	switch (In)
	{
	case MMBZ_Buy_Limit:
		return OrderSide::Buy;
	case MMBZ_Sell_Limit:
		return OrderSide::Sell;
	default:
		break;
	}
	return OrderSide::Buy;
}

void CJLB_2_TradeField(CJLB_STRUCT* pIn, TradeField* pOut)
{
	strcpy(pOut->ID, pIn->WTBH);
	strcpy(pOut->InstrumentID, pIn->ZQDM);
	pOut->Price = pIn->CJJG_;
	pOut->Qty = pIn->CJSL_;
	pOut->Date = pIn->CJRQ_;
	pOut->Time = pIn->CJSJ_;
	pOut->Side = MMBZ_2_OrderSide(pIn->MMBZ_);


	strcpy(pOut->TradeID, pIn->CJBH);

	pOut->Commission = pIn->YJ_ + pIn->YHS_ + pIn->GHF_ + pIn->CJF_;

	pOut->OpenClose = pOut->Side == OrderSide::Buy ? OpenCloseType::Open : OpenCloseType::Close;
	pOut->HedgeFlag = HedgeFlagType::Speculation;
	
}

void WTLB_2_OrderField_0(WTLB_STRUCT* pIn, OrderField* pOut)
{
	strcpy(pOut->ID, pIn->WTBH);
	strcpy(pOut->InstrumentID, pIn->ZQDM);
	pOut->Price = pIn->WTJG_;
	pOut->Qty = pIn->WTSL_;
	pOut->Date = pIn->WTRQ_;
	pOut->Time = pIn->WTSJ_;
	pOut->Side = MMBZ_2_OrderSide(pIn->MMBZ_);

	int wtfs = BJFS_2_WTFS(pIn->BJFS);
	pOut->Type = WTFS_2_OrderType(wtfs);
	pOut->TimeInForce = WTFS_2_TimeInForce(wtfs);

	pOut->Status = ZTSM_2_OrderStatus(pIn->ZTSM_);
	pOut->ExecType = ZTSM_2_ExecType(pIn->ZTSM_);

	pOut->OpenClose = pOut->Side == OrderSide::Buy ? OpenCloseType::Open : OpenCloseType::Close;
	pOut->HedgeFlag = HedgeFlagType::Speculation;

	strcpy(pOut->Account, pIn->GDDM);

	pOut->AvgPx = pIn->CJJG_;
	pOut->CumQty = pIn->CJSL_;
	pOut->LeavesQty = pIn->WTSL_ - pIn->CJSL_ - pIn->CDSL_;

	strcpy(pOut->Text, pIn->ZTSM);
}

int OrderType_2_WTFS(OrderType In)
{
	switch (In)
	{
	case Market:
	case Stop:
	case MarketOnClose:
	case TrailingStop:
		return WTFS_Five_IOC;// 只推荐使用五档模拟市价
	case Limit:
	case StopLimit:
	case TrailingStopLimit:
		return WTFS_Limit;
	case Pegged:
	default:
		return WTFS_Limit;
	}
}


void OrderField_2_Order_STRUCT(OrderField* pIn, Order_STRUCT* pOut)
{
	strcpy(pOut->ZQDM, pIn->InstrumentID);
	pOut->Price = pIn->Price;
	pOut->Qty = pIn->Qty;
	pOut->WTFS = OrderType_2_WTFS(pIn->Type);
	pOut->RZRQBS = RZRQBS_NO;

	// 这个地方后期要再改，因为没有处理基金等情况
	switch (pIn->Type)
	{
	case OrderType::Market:
		if (pIn->Side == OrderSide::Buy)
			pOut->MMBZ = MMBZ_Buy_Market;
		else
			pOut->MMBZ = MMBZ_Sell_Market;
		break;
	case OrderType::Limit:
		if (pIn->Side == OrderSide::Buy)
			pOut->MMBZ = MMBZ_Buy_Limit;
		else
			pOut->MMBZ = MMBZ_Sell_Limit;
		break;
	}
}