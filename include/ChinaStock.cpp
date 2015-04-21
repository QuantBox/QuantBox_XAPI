#include "stdafx.h"
#include "ChinaStock.h"

#include <stdlib.h>

// 上海证券交易所证券代码分配规则
// http://wenku.baidu.com/link?url=fhnAW62VTXqHTn8p9xlyXKa_oDIfR2xIxBF3y_fryeoFnz7MFbeWJbMLx4n1H61ERFnhr6PtaxF_j01x8iIT0wArZzrBtABRysi-KEpBa9S
InstrumentType InstrumentID_2_InstrumentType_SSE(char* pIn)
{
// 只有6位，8位的期权已经提前过滤
	int prefix1 = atoi(pIn) / 100000;
	int prefix3 = atoi(pIn) / 1000;
	switch (prefix1)
	{
	case 0:
		switch (prefix3)
		{
		case 0:
			return InstrumentType::Index;
		case 9:
		case 10:
		case 90:
		case 99:
			return InstrumentType::Bond;
		default:
			return InstrumentType::Index;
		}
	case 1:
		return InstrumentType::Bond;
	case 2:
		return InstrumentType::Bond;
	case 3:
		return InstrumentType::Future;
	case 5:
		switch (prefix3)
		{
		case 500:
		case 510:
		case 519:
		case 521:
		case 522:
		case 523:
		case 524:
			return InstrumentType::ETF;
		case 580:
		case 582:
			return InstrumentType::Option;
		default:
			return InstrumentType::ETF;
		}
	case 6:
		return InstrumentType::Stock;
	case 9:
		return InstrumentType::Stock;
	default:
		return InstrumentType::Synthetic;
	}
}

// 深圳证券交易所证券代码编码方案
// http://wenku.baidu.com/view/e41fba85ec3a87c24028c416.html
InstrumentType InstrumentID_2_InstrumentType_SZE(char* pIn)
{
// 只有6位，取前2
	int prefix1 = atoi(pIn) / 100000;
	int prefix2 = atoi(pIn) / 10000;
	switch (prefix2)
	{
	case 0:
	case 7:
	case 9:
		return InstrumentType::Stock;
	case 3:
	case 8:
		return InstrumentType::Option;
	case 10:
	case 11:
	case 12:
	case 13:
		return InstrumentType::Bond;
	case 17:
	case 18:
		return InstrumentType::ETF;
	case 20:
	case 30:
		return InstrumentType::Stock;
	case 27:
	case 37:
		return InstrumentType::Stock;
	case 28:
	case 38:
		return InstrumentType::Option;
	case 39:
		return InstrumentType::Index;
	default:
		return InstrumentType::Synthetic;
	}
}

PriceType InstrumentID_2_PriceTick_SSE(char* pIn)
{
	// 只有6位，8位的期权已经提前过滤
	int prefix1 = atoi(pIn) / 100000;
	int prefix3 = atoi(pIn) / 1000;
	switch (prefix1)
	{
	case 0:
		return 0.01;
	case 1:
		return 0.01;
	case 2:
		return 0.001;
	case 3:
		return 0.01;
	case 5:
		return 0.001;
	case 6:
		return 0.01;
	case 9:
		return 0.001;
	case 100:
		return 0.001;
	case 900:
		return 0.001;
	default:
		return 0.01;
	}

	return 0.01;
}

PriceType InstrumentID_2_PriceTick_SZE(char* pIn)
{
	// 只有6位，取前2
	int prefix1 = atoi(pIn) / 100000;
	int prefix2 = atoi(pIn) / 10000;
	switch (prefix2)
	{
	case 0:
	case 7:
	case 9:
		return 0.01;
	case 3:
	case 8:
		return 0.001;
	case 10:
	case 11:
	case 12:
	case 13:
		return 0.001;
	case 17:
	case 18:
		return 0.001;
	case 20:
	case 30:
		return 0.01;
	case 27:
	case 37:
		return 0.01;
	case 28:
	case 38:
		return 0.001;
	case 39:
		return 0.01;
	default:
		return 0.01;
	}
}
