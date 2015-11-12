#pragma once

#ifndef _TDX_ENUM_H_
#define _TDX_ENUM_H_

#define COL_EACH_ROW	(64) //每行多少例，相当重要

// 125_帐号类别


// 130_买卖标志
// 下单时使用买卖标志来指定报单类型
// 查询时，单子只简单的表示了买卖方向，是否要启用委托类别？
#define MMBZ_Buy_Limit			0	// 限价买
#define MMBZ_Sell_Limit			1	// 限价卖
#define MMBZ_Cancel				2	// 撤买/撤卖
#define MMBZ_3					3	// 融券
#define MMBZ_Buy_Market			67	// 市价买
#define MMBZ_Sell_Market		68	// 市价卖
#define MMBZ_Creation			79	// 基金申购
#define MMBZ_Redemption			80	// 基金赎回
#define MMBZ_Merge				12	// 基金合并
#define MMBZ_Split				13	// 基金分拆

// 5565_委托类别
#define WTLB_MM	0;	// 买卖
#define WTLB_CD	1;	// 撤单

// 131_委托类别
// 3|投票
// 3|融券
// 0|买入
// 1|卖出

// 166_委托方式
// 上海只有046,深圳只有012345,所以市价只发4最简单
// 0	限1，买卖
// 1	对
// 2	本
// 3	剩 撤
// 4	五 撤
// 5	全 撤
// 6	转
#define WTFS_Limit				0 // 限价,限价委托,买卖
#define WTFS_Best_Reverse		1 // 对手方最优价格委托,对方最优价格,对手方最优价格委托,
#define WTFS_Best_Forward		2 // 本方最优价格委托,本方最优价格,本方最优价格委托,
#define WTFS_IOC				3 // 即时成交剩余撤销委托,即时成交剩余撤销,即时成交剩余撤销委托,
#define WTFS_Five_IOC			4 // 五档即时成交剩余撤销,五档即成剩撤,最优五档即时成交剩余撤消委托,
#define WTFS_FOK				5 // 全额成交或撤销委托,全额成交或撤销,全额成交或撤销委托,
#define WTFS_Five_Limit			6 // 五档即时成交剩余转限,五档即成转限价,最优五档即时成交剩余转限价委托,



// 281_融资融券标识
#define RZRQBS_NO				0	// 非融资融券
#define RZRQBS_YES				1	// 融资融券

// 147_状态说明
#define ZTSM_NotSent			0	// 0-未申报
#define ZTSM_1					1	// 
#define ZTSM_New				2	// 2-已申报未成交,未成交,已报
#define ZTSM_Illegal			3	// 3-非法委托
#define ZTSM_4					4	//
#define ZTSM_PartiallyFilled	5	// 5-部分成交
#define ZTSM_AllFilled			6	// 6-全部成交,已成,全部成交
#define ZTSM_PartiallyCancelled	7	// 部撤，这是猜的，需要以后修正
#define ZTSM_AllCancelled		8	// 8-全部撤单,已撤,全部撤单
#define ZTSM_CancelRejected		9	// 9-撤单未成					只会出现撤单记录中

// 已成,部成,废单,已撤,部撤

#endif