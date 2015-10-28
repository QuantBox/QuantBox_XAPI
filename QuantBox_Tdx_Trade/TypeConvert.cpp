#include "stdafx.h"
#include "TypeConvert.h"
#include "../include/Tdx/tdx_enum.h"

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

void CJLB_2_TradeField(CJLB_STRUCT* pIn, TradeField* pOut)
{
	strcpy(pOut->ID, pIn->WTBH);
	strcpy(pOut->InstrumentID, pIn->ZQDM);
	pOut->Price = pIn->CJJG_;
	strcpy(pOut->TradeID, pIn->CJBH);

	pOut->Commission = pIn->YJ_ + pIn->YHS_ + pIn->GHF_ + pIn->CJF_;

	pOut->OpenClose = OpenCloseType::Open;
	pOut->HedgeFlag = HedgeFlagType::Speculation;
	pOut->Side = OrderSide::Buy;
	pOut->Time = pIn->CJSJ_;
}

void WTLB_2_OrderField_0(WTLB_STRUCT* pIn, OrderField* pOut)
{
	strcpy(pOut->ID, pIn->WTBH);

	pOut->Price = pIn->WTJG_;
	pOut->Qty = pIn->WTSL_;

	strcpy(pOut->Account, pIn->GDDM);

	pOut->Time = pIn->WTSJ_;

}
