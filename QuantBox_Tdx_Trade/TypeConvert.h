#pragma once

#include "../include/ApiStruct.h"

#include "../include/Tdx/tdx_struct.h"

// 两种情况：
// 1.完全重建
// 2.部分重建
int BJFS_2_WTFS(char* pIn);

void CJLB_2_TradeField(CJLB_STRUCT* pIn, TradeField* pOut);

// 
void WTLB_2_OrderField_0(WTLB_STRUCT* pIn, OrderField* pOut);
//void WTLB_2_OrderField_1(WTLB_STRUCT* pIn, OrderField* pOut);

void OrderField_2_Order_STRUCT(OrderField* pIn, Order_STRUCT* pOut);
