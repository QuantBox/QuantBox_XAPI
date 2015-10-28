#include "stdafx.h"
#include "TypeConvert.h"
#include "../include/Tdx/tdx_enum.h"

// 将中文转成委托方式
int BJFS_2_WTFS(char* pIn)
{
	// 最大的问题是中文对应不上
	// 它影响了Type和TimeInForce
	if (strstr("限价,限价委托,", pIn))
	{
		return WTFS_Limit;
	}
	else
	{
		return WTFS_Five_IOC;
	}
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

	pOut->OpenClose = OpenCloseType::Open;
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

	strcpy(pOut->Account, pIn->GDDM);

	

}
