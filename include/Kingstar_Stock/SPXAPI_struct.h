#ifndef __SPXAPI_STRUCT_H__
#define __SPXAPI_STRUCT_H__

#include "SPXAPI_type.h"

#ifdef __cplusplus
extern "C" {
#endif

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifdef WIN32
#define __PACKED__
#pragma pack(push, 1)
#else
#ifdef __GNUC__
#define __PACKED__    __attribute__ ((packed))
#else
#ifdef HP_UX
#define __PACKED__
#pragma pack 1
#else
#define __PACKED__
#pragma options align = packed
#endif
#endif
#endif

#ifndef WIN32
#if defined(__i386__) && ! defined(__x86_64__)
#ifndef __stdcall
#define __stdcall __attribute__((stdcall))
#endif
#elif defined(__x86_64__)
#ifndef __stdcall
#define __stdcall
#endif
#endif
#endif

/** \defgroup Group13 1.3 枚举定义 */
/** @{ */

/**
 * @brief 日志级别
 * @details API记录日志的级别，设置为某一级别后，该级别及更严重级别的日志都会记录到日志文件中
 */
enum LOG_LEVEL
{
    LL_DEBUG           = 0,		///< 调试级别
    LL_INFO,					///< 信息级别(缺省选项)
    LL_WARNING,					///< 警告级别
    LL_ERROR,					///< 错误级别
    LL_FATAL,					///< 严重错误级别
};

/**
 * @brief ETX业务功能号枚举
 * @details VIP柜台ETX推送业务功能号
 */
enum ETX_APP_FUNCNO
{
	ETX_16203          = 16203,	///< 16203 - 成交撤单废单
	ETX_16204,					///< 16204 - 撤单的废单
};

/**
 * @brief API参数类型枚举
 * @details API参数类型
 */
enum PARAM_TYPE
{
	MAINSERVER_IP       = 0,	///< 主网关IP
	MAINSERVER_PORT,			///< 主网关端口
	BACKSERVER_IP,				///< 备网关IP
	BACKSERVER_PORT,			///< 备网关端口

	PROXY_TYPE,					///< socks代理类型
	PROXY_IP,					///< socks代理IP
	PROXY_PORT,					///< socks代理端口
	PROXY_USER,					///< socks5/http代理用户名
	PROXY_PASS,					///< socks5/http代理密码
};

/**
 * @brief 代理类型枚举
 * @details socks代理类型
 */
enum PROXYTYPE
{
	NOPROXY				= 0,	///< 没有代理
	SOCKS4,						///< sock4代理
	SOCKS5,						///< sock5代理
	SOCKS5_LOGON,				///< 带用户验证的sock5代理
	HTTP,						///< HTTP代理
	HTTP_LOGON,					///< 带用户验证的HTTP代理
};
/** @} */

/** \defgroup Group14 1.4 数据结构定义 */
/** @{ */
/// 无超时限制
#ifndef INFINITE
    #define INFINITE	0xFFFFFFFF
#endif

///查询最大返回数量
#define ZB_MAX_RETU		99999


/// 空行
#define EMPTY_ROW       -1
/// 空ID
#define EMPTY_ID        -1
/// 空NO
#define EMPTY_NO        -1


	///	ETX推送回调
	typedef void (CALLBACK* TOnReadPushData)(ETX_APP_FUNCNO FuncNO, void* pEtxPushData);

	/// 初始化参数
	typedef struct _STInitPara
    {
		TOnReadPushData     pOnReadPushData;            ///< ETX推送回调函数
		bool			    bWriteLog;					///< 是否记录日志信息
		LOG_LEVEL		    emLogLevel;					///< 日志级别，参见LOG_LEVEL
		int				    nTimeOut;					///< 超时时间，单位ms
    }__PACKED__ STInitPara, *PSTInitPara;

    /// 应答消息
    typedef struct _STRspMsg
    {
        TErrorNoType        error_no;                   ///< 错误代码（0为成功，其他为失败）
        TErrorMsgType       msg;                        ///< 错误信息
    }__PACKED__ STRspMsg, *PSTRspMsg;
    
    /// 交易员登录指令  //对应spx61
    typedef struct _STTraderLogin
    {
        TCustNoType         cust_no;                    ///< 客户号
        TCustPwdType        cust_pwd;                   ///< 客户密码
    } __PACKED__ STTraderLogin, *PSTTraderLogin;
    
    /// 登录应答
    typedef struct _STTraderLoginRsp
    {
        TCustNameType       cust_name;                  ///< 客户姓名
        TDateType           tx_date;                    ///< 交易日期
		TMarketCodeType     market_code;                ///< 市场代码
		THolderNoType       holder_acc_no;              ///< 股东帐号
    } __PACKED__ STTraderLoginRsp, *PSTTraderLoginRsp;

    /// Api信息
    typedef struct _STApiInfo
    {
        TApiVersionType     api_version;                ///< Api版本号
		TOrganizationNO		org_no;						///< 机构号
		TIPADDR				server_ip;					///< 网关IP
		TPort				server_port;				///< 网关端口
		TDateType			auth_date;					///< 授权到期日
    } __PACKED__ STApiInfo, *PSTApiInfo;

    /// 委托请求  
    typedef struct _STOrder
    {
        TSecCodeType        sec_code;                   ///< 证券代码
        TBSType             bs;                         ///< 买卖标记(1买2卖0申9赎U分拆P合并)
        TMarketOrderFlagType market_order_flag;         ///< 市价委托标记，市价委托和委托价格二选一
														/*
														2  深对手方最优价格委托		深圳 
														3  深本方最优价格委托		深圳 
														4  深即时成交剩余撤销委托	 
														5  沪深最优五档并撤销委托	上海、深圳 
														6  深全额成交或撤销委托		 
														7  沪最优五档并转限价委托	上海 
		                                                */
        TPriceType          price;                      ///< 委托价格
        TStockVolType       order_vol;                  ///< 委托数量
        TOrderPropType      order_prop;                 ///< 委托属性(预留)
    }__PACKED__ STOrder, *PSTOrder;

    /// 委托应答
    typedef struct _STOrderRsp
    {
        TBatchNoType        batch_no;                   ///< 批号
		TContractNoType     order_no;                   ///< 系统委托流水号
		TFundAmountType     forzen_amount;              ///< 委托冻结金额      
		TStockVolType       forzen_vol;                 ///< 委托冻结数量
		TErrorNoType        error_no;                   ///< 委托错误代码（0为成功，其他为失败）
        TErrorMsgType       err_msg;                    ///< 委托错误信息
    }__PACKED__ STOrderRsp, *PSTOrderRsp;

    /// 撤单请求 
    typedef struct _STOrderCancel
    {
		TMarketCodeType     market_code;                ///< 市场代码（按批撤必须填入）
		TCustNoType         cust_no;                    ///< 客户号
        TOrderCancelType    ordercancel_type;           ///< 撤单方式（0按批撤，1按单笔撤）
		TContractNoType     order_no;                   ///< 流水号
    }__PACKED__ STOrderCancel, *PSTOrderCancel;

	 /// 撤单应答
    typedef struct _STOrderCancelRsp
    {
		TErrorNoType        error_no;                   ///< 撤单错误代码（0为成功，其他为失败）
        TErrorMsgType       err_msg;                    ///< 撤单错误信息
    }__PACKED__ STOrderCancelRsp, *PSTOrderCancelRsp;
   
    /// 查询账户资金请求 
    typedef struct _STQueryFund
    {
	    TCustNoType         cust_no;                    ///< 客户号
        TCurrencyTypeType   currency_type;              ///< 币种(0-所有币种，1-人民币，2：美元，3：港币，4：港股港币) 
    }__PACKED__ STQueryFund, *PSTQueryFund;

    /// 账户资金信息
    typedef struct _STFundInfo
    {
        TCurrencyTypeType   currency_type;              ///< 币种
        TFundAmountType     deposite;                   ///< 资金余额
        TFundAmountType     buyable;                    ///< 可买资金余额
    }__PACKED__ STFundInfo, *PSTFundInfo;

    /// 查询持仓请求 
    typedef struct _STQueryPosition
    {
	    TCustNoType         cust_no;                    ///< 客户号
        TMarketCodeType     market_code;                ///< 市场代码(为0时查询所有市场)
        TSecCodeType        sec_code;                   ///< 证券代码(为空时查询所有证券代码) 
    }__PACKED__ STQueryPosition, *PSTQueryPosition;

    /// 持仓信息
    typedef struct _STPositionInfo
    {
        TMarketCodeType     market_code;                ///< 市场代码
        TSecCodeType        sec_code;                   ///< 证券代码
		TSecNameType        sec_name;                   ///< 证券简称
		TStockVolType       buy_vol;                    ///< 当日买入成交数量
		TStockVolType       sell_vol;                   ///< 当日卖出成交数量
        TStockVolType       total_vol;                  ///< 当前总持仓（含超限库存、当日买卖）
        TStockVolType       avail_vol;                  ///< 当前可用余额
		TFundAmountType     market_value;               ///< 市值
        TFundAmountType     total_cost;                 ///< 持仓成本
        TPriceType          last_price;                 ///< 最新价
        TFundAmountType     float_profit;               ///< 浮动盈亏
    }__PACKED__ STPositionInfo, *PSTPositionInfo;
   
   ///查询资产
	typedef struct _STQueryAssest
    {
		TCustNoType         cust_no;                    ///< 客户号
        TCurrencyTypeType   currency_type;              ///< 币种(必填，1-人民币，2：美元，3：港币，4：港股港币) 
    }__PACKED__ STQueryAssest, *PSTQueryAssest;

    /// 查询资产
    typedef struct _STAssestInfo
    {
       TFundAmountType      total_amount;               ///< 总资产(含超限库存、含在途资金)
    }__PACKED__ STAssestInfo, *PSTAssestInfo;
	
	
	 /// 查询帐务
    typedef struct _STQueryAccount
    {
        TCustNoType         cust_no;                    ///< 客户号
        TMarketCodeType     market_code;                ///< 市场代码(为0时查询所有市场)
		TCurrencyTypeType   currency_type;              ///< 币种
		TDateType           begin_date;                 ///< 开始日期(为空查询当日)
		TDateType           end_date;                   ///< 结束日期(为空查询当日)
		TQueryType          query_type;                 ///< 查询类别(0增量，1全量) 
    }__PACKED__ STQueryAccount, *PSTQueryAccount;

    /// 帐务信息
    typedef struct _STAccountInfo
    {
	    TDateType           date;                 	    ///< 发生日期 
	    TMarketCodeType     market_code;                ///< 市场代码
		THolderNoType       holder_acc_no;              ///< 股东帐号
        TSecCodeType        sec_code;                   ///< 证券代码
	    TNoteType2          note;        				///< 摘要中文名称
		TBSType             bs;                         ///< 买卖类型(买、卖、转、贷)
		TCurrencyTypeType   currency_type;              ///< 币种
		TFundAmountType     fund_change_amount;         ///< 变动金额
		TFundAmountType     fund_amount;        	    ///< 本次余额
		TPriceType          done_price;                 ///< 成交价格
        TStockVolType       done_vol;                   ///< 成交数量
		TFundAmountType     done_amount;                ///< 成交金额
		TTimeType           done_time;                  ///< 成交时间
		TDoneNoType         done_no;                    ///< 成交编号
		TFundAmountType     trade_fee;                  ///< 交易费用(佣金、过户费、印花税、其他费用)
		TFundAmountType     commission;                 ///< 佣金
		TFundAmountType     chg_owner_fee;              ///< 过户费
		TFundAmountType     stamp_tax;                  ///< 印花税
		TFundAmountType     other_fee;                  ///< 其他费用
		TOrderNoType        order_no;                   ///< 系统委托流水号
    }__PACKED__ STAccountInfo, *PSTAccountInfo;
	
    /// 查询委托请求
    typedef struct _STQueryOrder
    {
		TCustNoType         cust_no;                    ///< 客户号
        TMarketCodeType     market_code;                ///< 市场代码(为0时查询所有市场)
        TSecCodeType        sec_code;                   ///< 证券代码(为空时查询所有证券代码)		
		TBatchNoType        batch_no;                   ///< 批号(非EMPTY_NO时按批号查询)，单笔查询时置为0
		TContractNoType     order_no;                   ///< 系统委托流水号，按批查询时置为0
        TQueryOrderPropType query_order_prop;           ///< 查询委托属性(0为查询所有,1为仅查询可撤单委托)
		TQueryType          query_type;                 ///< 查询类别(0增量，1全量)  
    }__PACKED__ STQueryOrder, *PSTQueryOrder;
    
	/// 委托信息(委托应答、委托查询应答)
    typedef struct _STOrderInfo
    {
        TMarketCodeType     market_code;                ///< 市场代码
		THolderNoType       holder_acc_no;              ///< 股东帐号
		TBSFLAG             bs;                         ///< 买卖方向(1买2卖)
        TSecCodeType        sec_code;                   ///< 证券代码
		TSecNameType        sec_name;                   ///< 证券简称
        TPriceType          price;                      ///< 委托价格
        TStockVolType       order_vol;                  ///< 委托数量
        TBatchNoType        batch_no;                   ///< 批号
        TContractNoType     order_no;                   ///< 系统委托流水号		
        TOrderStatusType    order_status;               ///< 委托状态
		TFundAmountType     frozen_amount;              ///< 冻结金额
        TStockVolType       frozen_vol;                 ///< 冻结数量
        TPriceType          done_price;                 ///< 成交价格
        TStockVolType       done_vol;                   ///< 成交数量
        TStockVolType       cancel_vol;                 ///< 已撤销数量
		TDateType           order_date;                 ///< 委托日期  
        TTimeType           order_time;                 ///< 委托时间   
    }__PACKED__ STOrderInfo, *PSTOrderInfo;
	
    /// 查询成交请求
    typedef struct _STQueryDone
    {
		TCustNoType         cust_no;                    ///< 客户号
        TMarketCodeType     market_code;                ///< 市场代码(为0时查询所有市场)
        TSecCodeType        sec_code;                   ///< 证券代码(为空时查询所有证券代码)
		TQueryType          query_type;                 ///< 查询类别(0增量，1全量)
    }__PACKED__ STQueryDone, *PSTQueryDone;
    
	 /// 成交信息(成交返回、成交查询应答)
    typedef struct _STDoneInfo
    {
        TMarketCodeType     market_code;                ///< 市场代码
        TBSFLAG             bs;                         ///< 买卖方向(1买2卖)
        TSecCodeType        sec_code;                   ///< 证券代码
		TSecNameType        sec_name;                   ///< 证券简称
        //TBatchNoType        batch_no;                 ///< 批号
        TContractNoType     order_no;                   ///< 系统委托流水号
        TPriceType          done_price;                 ///< 成交价格
        TStockVolType       done_vol;                   ///< 成交数量
        TFundAmountType     done_amount;                ///< 成交金额
        TTimeType           done_time;                  ///< 成交时间
		TDoneNoType         done_no;                    ///< 成交编号
		TFundAmountType     forzen_amount;              ///< 解冻资金金额
		TStockVolType       forzen_vol;                 ///< 解冻证券数量
    }__PACKED__ STDoneInfo, *PSTDoneInfo;

    /// 证券基本信息查询
    typedef struct _STQuerySecCode
    {
        TMarketCodeType     market_code;                ///< 市场代码（针对特定市场查询，不支持为0时查询所有）
        TSecCodeType        sec_code;                   ///< 证券代码（针对特定证券查询，不支持为空时查询所有）
    }__PACKED__ STQuerySecCode, *PSTQuerySecCode;
    
    /// 证券基本信息
    typedef struct _STSecCodeInfo
    {
        TMarketCodeType     market_code;                ///< 市场代码
        TSecCodeType        sec_code;                   ///< 证券代码
        TSecNameType        sec_name;                   ///< 证券简称
        TSecVarietyType     sec_variety;                ///< 证券类别
        TSecStatusType      sec_status;                 ///< 证券状态（1-停牌标志）
        TPriceType          raise_limit_price;          ///< 涨停价格
        TPriceType          fall_limit_price;           ///< 跌停价格
    }__PACKED__ STSecCodeInfo, *PSTSecCodeInfo;
    
    /// ETF基本信息查询
    typedef struct _STQueryEtfBaseInfo
    {
        TMarketCodeType     market_code;                ///< 市场代码(为0时查询所有市场)
        TSecCodeType        sgsh_tx_code;				///< 申购赎回证券代码(为空时查询所有申购赎回证券代码)
    }__PACKED__ STQueryEtfBaseInfo, *PSTQueryEtfBaseInfo;
    
    /// ETF基本信息
    typedef struct _STEtfBaseInfo
    {
        TSecCodeType        sgsh_tx_code;               ///< 基金申购赎回证券代码
        TSecCodeType        bs_code;                    ///< 基金买卖证券代码
        TEtfNameType        etf_name;                   ///< ETF简称
        TUnitType           sgsh_min_unit;              ///< 申购赎回最小份额
        TFundAmountType     estimate_cash_bal;          ///< 当日预估现金余额
        TFundAmountType     cash_bal;                   ///< 上日现金余额差额
        TRatioType          max_cash_ratio;             ///< 现金替代比例上限
        TFundAmountType     etf_nav;                    ///< 参考基金净值
        TRecordNumType      record_num;                 ///< 篮子股票个数
		TEtfPropType        etf_proptype;               ///< ETF属性
    }__PACKED__ STEtfBaseInfo, *PSTEtfBaseInfo;
    
    /// ETF篮子信息查询
    typedef struct _STQueryEtfBasketInfo
    {
        TMarketCodeType     market_code;                ///< 市场代码(为0时查询所有市场)
        TSecCodeType        sgsh_tx_code;				///< 申购赎回证券代码(为空时查询所有申购赎回证券代码)
    }__PACKED__ STQueryEtfBasketInfo, *PSTQueryEtfBasketInfo;

    /// ETF篮子信息
    typedef struct _STEtfBasketInfo
    {
        TMarketCodeType     market_code;                ///< 市场代码
        TSecCodeType        sec_code;                   ///< 证券代码
        TSecNameType        sec_name;                   ///< 证券简称
        TStockVolType       stock_vol;                  ///< 股票数量
        TCashReplFlagType   cash_repl_flag;             ///< 现金替代标志
        TRatioType          over_ratio;                 ///< 溢价比例
        TFundAmountType     cash_repl_amt;              ///< 现金替代金额
    }__PACKED__ STEtfBasketInfo, *PSTEtfBasketInfo;
	
	// 16203（成交撤单废单）推送数据结构体
	typedef struct _ST16203PushData
	{
		TAppFunctionNO		app_func_no;				///< 业务功能号 16203
		TCustNoType         cust_no;                    ///< 客户号
		TMarketCodeType     market_code;                ///< 市场代码
		THolderNoType       holder_acc_no;              ///< 股东代码
        TSecCodeType        sec_code;                   ///< 证券代码
		TSecNameType        sec_name;                   ///< 证券简称
        TBatchNoType        batch_no;                   ///< 委托批号
        TContractNoType     order_no;                   ///< 系统委托流水号
		TOrderStatusType    order_status;               ///< 委托状态
		TOrderStatusName	order_status_name;			///< 委托状态名称
        TPriceType          done_price;                 ///< 成交价格（该笔委托的平均成交价格）
        TStockVolType       done_vol;                   ///< 成交数量（该笔委托的成交数量）
        TTimeType           done_time;                  ///< 成交时间
		TDoneNoType         done_no;                    ///< 交易所成交编号（该笔成交的，若撤单或废单则为"-1"）
		TPriceType          unit_done_price;            ///< 单笔成交价格（该笔成交的成交数量）
        TStockVolType       unit_done_vol;              ///< 单笔成交数量（该笔成交的成交价格）
		TInvalidOrderReason invalid_order_reason;		///< 废单原因
		TDoneIDType			done_id;					///< 成交ID
	}__PACKED__ ST16203PushData, *PST16203PushData;

	//16204（撤单的废单）推送数据结构体
	typedef struct _ST16204PushData
	{
		TAppFunctionNO		app_func_no;				///< 业务功能号 16204
		TCustNoType         cust_no;                    ///< 客户号
		TMarketCodeType     market_code;                ///< 市场代码
		THolderNoType       holder_acc_no;              ///< 股东代码
        TSecCodeType        sec_code;                   ///< 证券代码
		TSecNameType        sec_name;                   ///< 证券简称
        TBatchNoType        batch_no;                   ///< 委托批号
        TContractNoType     order_no;                   ///< 系统委托流水号
		TOrderStatusType    order_status;               ///< 委托状态
		TOrderStatusName	order_status_name;			///< 委托状态名称
		TInvalidOrderReason invalid_order_reason;		///< 废单原因
		TTimeType           done_time;                  ///< 成交时间
	}__PACKED__ ST16204PushData, *PST16204PushData;

/** @} */

#ifdef WIN32
#pragma pack(pop)
#else
#ifdef __GNUC__
#else
#ifdef HP_UX
#pragma pack 0
#else
#pragma options align = reset
#endif
#endif
#endif
#undef __PACKED__
#ifdef __cplusplus
}
#endif

#endif //__SPXAPI_STRUCT_H__
