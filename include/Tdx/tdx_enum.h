#pragma once

#ifndef _TDX_ENUM_H_
#define _TDX_ENUM_H_

#define COL_EACH_ROW	(64) //每行多少例，相当重要

// 125_帐号类别
//#define EXCHANGE_SZE_A	0	// 深A
//#define EXCHANGE_SSE_A	1	// 沪A

// 130_买卖标志
#define MMBZ_Buy_Limit			0	// 限价买
#define MMBZ_Sell_Limit			1	// 限价卖
#define MMBZ_Buy_Limit_Cancel	2	// 撤买
#define MMBZ_3					3	// 融券
#define MMBZ_Buy_Market			67	// 市价买
#define MMBZ_Sell_Market		68	// 市价卖
#define MMBZ_Creation			79	// 基金申购
#define MMBZ_Redemption			80	// 基金赎回
#define MMBZ_Merge				12	// 基金合并
#define MMBZ_Split				13	// 基金分拆

// 131_委托类别
// 3|投票
// 3|融券
// 0|买入
// 1|卖出

// 166_委托方式
// 上海只有046,深圳只有012345,所以市价只发4最简单
#define WTFS_Limit				0 // 限价
#define WTFS_Best_Reverse		1 // 对手方最优价格委托
#define WTFS_Best_Forward		2 // 本方最优价格委托
#define WTFS_IOC				3 // 即时成交剩余撤销委托
#define WTFS_Five_IOC			4 // 五档即时成交剩余撤销
#define WTFS_FOK				5 // 全额成交或撤销委托
#define WTFS_Five_Limit			6 // 五档即时成交剩余转限

// 281_融资融券标识
#define RZRQBS_NO				0	// 非融资融券
#define RZRQBS_YES				1	// 融资融券


#define ORDER_STATUS_0					0	// 0-未申报
#define ORDER_STATUS_1					1	// 
#define ORDER_STATUS_New				2	// 2-已申报未成交
#define ORDER_STATUS_Illegal			3	// 3-非法委托
#define ORDER_STATUS_AllFilled			6	// 6-全部成交
#define ORDER_STATUS_AllCancelled		8	// 8-全部撤单

#endif