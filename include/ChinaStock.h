#ifndef _CHINA_STOCK_H_
#define _CHINA_STOCK_H_

#include "../include/ApiStruct.h"

// 上海证券交易所证券代码分配规则
// http://wenku.baidu.com/link?url=fhnAW62VTXqHTn8p9xlyXKa_oDIfR2xIxBF3y_fryeoFnz7MFbeWJbMLx4n1H61ERFnhr6PtaxF_j01x8iIT0wArZzrBtABRysi-KEpBa9S

// 深圳证券交易所证券代码编码方案
// http://wenku.baidu.com/view/e41fba85ec3a87c24028c416.html


InstrumentType InstrumentID_2_InstrumentType_SSE(int In);
InstrumentType InstrumentID_2_InstrumentType_SZE(int In);
InstrumentType InstrumentID_2_InstrumentType_NEEQ(int In);
PriceType InstrumentID_2_PriceTick_SSE(int In);
PriceType InstrumentID_2_PriceTick_SZE(int In);
PriceType InstrumentID_2_PriceTick_NEEQ(int In);
#endif