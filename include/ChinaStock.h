#ifndef _CHINA_STOCK_H_
#define _CHINA_STOCK_H_

#include "../include/ApiStruct.h"

InstrumentType InstrumentID_2_InstrumentType_SSE(char* pIn);
InstrumentType InstrumentID_2_InstrumentType_SZE(char* pIn);
PriceType InstrumentID_2_PriceTick_SSE(char* pIn);
PriceType InstrumentID_2_PriceTick_SZE(char* pIn);
#endif