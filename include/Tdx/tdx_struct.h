#pragma once

#ifndef _TDX_STRUCT_H_
#define _TDX_STRUCT_H_


// 字段信息,通达信内部定义的，非自定义
struct FieldInfo_STRUCT
{
	short FieldID;		// 字段ID
	char a;				// 只有a和1两种情况
	char b;				// 怎么一直都是2？
	char Len;			// 好像目前看到最大的是32
	char d;
	char e;
	char FieldName[25];	// 字段名字
};

// 错误
struct Error_STRUCT
{
	int ErrType;
	int ErrCode;
	char ErrInfo[256];
};

struct Order_STRUCT
{
	char ZJZH[32];	// 121_资金帐号
	char GDDM[32];	// 123_股东代码
	char ZQDM[32];	// 140_证券代码
	int ZHLB;		// 125_帐号类别
	int RZRQBS;		// 281_融资融券标识
	double Price;	// 145_委托价格
	long Qty;		// 144_委托数量
	long MMBZ;		// 130_买卖标志
	long WTFS;		// 166_委托方式

	// 下完单后用来回填
	char WTBH[32];	// 146_委托编号
};

// 股东列表
struct GDLB_STRUCT
{
	char GDDM[32];	// 123_股东代码
	char GDMC[32];	// 124_股东名称
	char ZJZH[32];	// 121_资金帐号
	char ZHLB[2];	// 125_帐号类别
	char RZRQBS[2];	// 281_融资融券标识
	char BLXX[32];	// 1213_保留信息

	int ZHLB_;		// 125_帐号类别
	int RZRQBS_;	// 281_融资融券标识
};

//
////////////////////////////////////////////////////////////////////////////
//// 以下的在整理完后要删除
//

//
//// 当日委托/历史委托=委托列表
//struct WTLB_STRUCT
//{
//	char WTRQ[64];	// 委托日期
//	char WTSJ[64];	// 委托时间
//	char GDDM[64];  // 股东代码
//	char ZQDM[64];  // 证券代码
//	char ZQMC[64];  // 证券名称
//	char MMBZ[64];  // 买卖标志
//	char WTJG[64];  // 委托价格
//	char WTSL[64];  // 委托数量
//	char CJJG[64];  // 成交价格
//	char CJSL[64];  // 成交数量
//	char CDSL[64];  // 撤单数量
//	char WTBH[64];  // 委托编号
//	char BJFS[64];  // 报价方式
//	char ZTSM[64];  // 状态说明
//	char DJZJ[64];  // 冻结资金
//
//	double WTJG_;
//	int WTSL_;
//	double CJJG_;
//	int CJSL_;
//	int CDSL_;
//	double DJZJ_;
//};
//
//// 股份列表
//struct GFLB_STRUCT
//{
//	char ZQDM[64];  // 证券代码
//	char ZQMC[64];  // 证券名称
//	char ZQSL[64];  // 证券数量
//	char KMSL[64];  // 可卖数量
//	char TBCBJ[64];  // 摊薄成本价
//	char DQJ[64];  // 当前价
//	char ZXSZ[64];  // 最新市值
//	char TBFDYK[64];  // 摊薄浮动盈亏
//	char SXYK[64];  // 实现盈亏
//	char CKYKBL[64];  // 参考盈亏比例
//	char DJSL[64];  // 冻结数量
//	char GDDM[64];  // 股东代码
//	char SCLB[64]; // 市场类别
//	char JYSMC[64];  // 交易所名称
//
//	int ZQSL_;
//	int KMSL_;
//	double TBCBJ_;
//};
//
//// 资金余额
//struct ZJYE_STRUCT
//{
//	char HBLX[64];		// 货币类型
//	char ZJYE[64];		// 资金余额
//	char KYZJ[64];		// 可用资金
//	char KQZJ[64];		// 可取资金
//	char ZC[64];		// 资产
//
//	double CKSZ_;		// 参考市值
//	double YK_;			// 盈亏
//
//	double ZJYE_;
//	double KYZJ_;
//	double KQZJ_;
//	double ZC_;
//};
//
//
//
//// 委托结果
//struct WTJG_STRUCT
//{
//	char WTBH[64];	// 委托编号
//
//	// 返回的结果中没有，这里自己填上，这样就可以立即撤单，而不用等查询了
//	char GDDM[64];  // 股东代码
//};

// 登录结果
struct DLJG_STRUCT
{
	char COL1[128];
	char COL2[128];
	char COL3[128];
};

#endif


