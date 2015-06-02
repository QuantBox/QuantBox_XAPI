#include "stdafx.h"
#include "ApiProcess.h"

void InitBidAsk(DepthMarketDataNField* pField)
{
	pField->Size = sizeof(DepthMarketDataNField);
	pField->BidCount = 0;
}

int AddBid(DepthMarketDataNField* pField, PriceType price, VolumeType volume, VolumeType Count)
{
	char* p = ((char*)pField) + pField->Size;
	DepthField* pDF = (DepthField*)p;
	pDF->Price = price;
	pDF->Size = volume;
	pDF->Count = Count;
	pField->Size += sizeof(DepthField);
	++pField->BidCount;

	return 0;
}

int AddAsk(DepthMarketDataNField* pField, PriceType price, VolumeType volume, VolumeType Count)
{
	char* p = ((char*)pField) + pField->Size;
	DepthField* pDF = (DepthField*)p;
	pDF->Price = price;
	pDF->Size = volume;
	pDF->Count = Count;
	pField->Size += sizeof(DepthField);

	return 0;
}

DepthField* GetBid(DepthMarketDataNField* pField, int Pos)
{
	if (Pos <= 0 || Pos>pField->BidCount)
		return nullptr;

	char* p = ((char*)pField) + sizeof(DepthMarketDataNField);
	DepthField* pDF = (DepthField*)p;
	return pDF + Pos - 1;
}

DepthField* GetAsk(DepthMarketDataNField* pField, int Pos)
{
	int count = (pField->Size - sizeof(DepthMarketDataNField)) / sizeof(DepthField);
	int AskCount = count - pField->BidCount;

	if (Pos <= 0 || Pos>AskCount)
		return nullptr;

	char* p = ((char*)pField) + sizeof(DepthMarketDataNField);
	DepthField* pDF = (DepthField*)p;
	return pDF + pField->BidCount + (Pos - 1);
}

char* ExchangeType_2_String(ExchangeType exchange)
{
	switch (exchange)
	{
	case Undefined_:
		return "Undefined";
	case SHFE:
		return "SHFE";
	case DCE:
		return "DCE";
	case CZCE:
		return "CZCE";
	case CFFEX:
		return "CFFEX";
	case INE:
		return "INE";
	case SSE:
		return "SSE";
	case SZE:
		return "SZE";
	case NEEQ:
		return "NEEQ";
	case HKEx:
		return "HKEx";
	default:
		return "Undefined";
	}
}