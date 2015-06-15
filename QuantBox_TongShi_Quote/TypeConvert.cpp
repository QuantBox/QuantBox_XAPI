#include "stdafx.h"
#include "TypeConvert.h"

ExchangeType Market_2_ExchangeType(WORD In)
{
	switch (In)
	{
	case SH_MARKET_EX:
		return ExchangeType::SSE;
	case SZ_MARKET_EX:
		return ExchangeType::SZE;
	case HK_MARKET_EX:
		return ExchangeType::HKEx;
	case SB_MARKET_EX:
		return ExchangeType::NEEQ;
	default:
		return ExchangeType::Undefined_;
	}
}

char* Market_2_ExchangeID(WORD In)
{
	switch (In)
	{
	case SH_MARKET_EX:
		return "SSE";
	case SZ_MARKET_EX:
		return "SZE";
	case HK_MARKET_EX:
		return "HKEx";
	case EB_MARKET_EX:
		return "EB";
	case SB_MARKET_EX:
		return "NEEQ";
	default:
		return "";
	}
}

char _OldSymbol_2_NewSymbol[18][2][7] = {
	{ "1A0001", "000001" }, // 上证指数
	{ "1A0002", "000002" }, // Ａ股指数
	{ "1A0003", "000003" }, // Ｂ股指数

	{ "1B0001", "000004" }, // 工业指数
	{ "1B0002", "000005" }, // 商业指数
	{ "1B0004", "000006" }, // 地产指数
	{ "1B0005", "000007" }, // 公用指数
	{ "1B0006", "000008" }, // 综合指数
	{ "1B0007", "000010" }, // 上证180
	{ "1B0008", "000011" }, // 基金指数
	{ "1B0009", "000012" }, // 国债指数
	{ "1B0010", "000013" }, // 企债指数

	{ "1B0015", "000015" }, // 红利指数
	{ "1B0016", "000016" }, // 上证50
	{ "1B0017", "000017" }, // 新综指

	{ "1C0002", "000015" }, // 红利指数
	{ "1C0003", "000016" }, // 上证50
	{ "1C0004", "000017" }, // 新综指
};

char* OldSymbol_2_NewSymbol(char* In,WORD In2)
{
	if (In2 != SH_MARKET_EX)
		return In;
	if (In[1] >= 'A')
	{
		for (int i = 0; i < 18; ++i)
		{
			if (strcmp(In, _OldSymbol_2_NewSymbol[i][0]) == 0)
			{
				return _OldSymbol_2_NewSymbol[i][1];
			}
		}
	}
	return In;
}
