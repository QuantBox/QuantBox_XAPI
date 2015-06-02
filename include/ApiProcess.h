#ifndef _API_PROCESS_H_
#define _API_PROCESS_H_

#include "../include/ApiStruct.h"

// 先对行情数据进行初始化，然后才能进行
void InitBidAsk(DepthMarketDataNField* pField);
// 必须先AddBid才能AddAsk，先添加的是买一，然后才是买二
int AddBid(DepthMarketDataNField* pField, PriceType price, VolumeType volume, VolumeType Count = 0);
// 先添加的是卖一，然后才是卖二
int AddAsk(DepthMarketDataNField* pField, PriceType price, VolumeType volume, VolumeType Count = 0);
// 从结构体中读出买档，Pos=1是买一
DepthField* GetBid(DepthMarketDataNField* pField, int Pos);
// 从结构体中读出卖档，Pos=1是卖一
DepthField* GetAsk(DepthMarketDataNField* pField, int Pos);

char* ExchangeType_2_String(ExchangeType exchange);

#endif