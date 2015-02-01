#ifndef __SPXAPI_TYPE_H__
#define __SPXAPI_TYPE_H__

#ifdef __cplusplus
extern "C" {
#endif

/** \defgroup Group11 1.1 数据长度定义 */
/** @{ */
/// API版本长度
#define API_VERSION_LEN				30
/// 错误信息长度
#define MSG_LEN						256
/// 客户号长度
#define CUST_NO_LEN					16
/// 股东代码长度
#define HOLDER_NO_LEN				16
/// 密码长度
#define PWD_LEN						33
/// 证券代码长度
#define SEC_CODE_LEN				20
/// 备注长度
#define NOTE_LEN					256
/// 备注长度
#define NOTE_LEN2					21
/// 柜台实例号长度
#define INSTANCE_ID_LEN				32
/// 客户名称长度
#define CUST_NAME_LEN				21
/// 证券名称长度
#define SEC_NAME_LEN				21
/// ETF名称长度
#define ETF_NAME_LEN				81
/// ETF属性长度
#define ETF_PROP_LEN				3
/// 买卖类型长度
#define BS_TYPE_LEN					3
/// 日期类型长度
#define DATE_TYPE_LEN				9
/// 委托流水号长度
#define ORDER_NO_LEN				17
/// 委托状态名称长度
#define ORDER_STATUS_NAME_LEN		21
/// 废单原因长度
#define INVALID_ORDER_REASON_LEN	7
/// 业务功能号长度
#define	APP_FUNC_NO_LEN				11
/// 网卡十六进制地址长度
#define MAC_ADDR_LEN				18
/// IP地址长度
#define IP_ADDR_LEN					20
/// 机构号长度
#define ORG_NO_LEN					21
/// 成交编号长度
#define DONE_NO_LEN					13
/// 代理用户名长度
#define PROXYUSER_LEN				100
/// 代理用户密码长度
#define PROXYPASS_LEN				100
/** @} */

/** \defgroup Group12 1.2 数据类型定义 */
/** @{ */
/// 布尔类型
#ifndef BOOL
    typedef int BOOL;
#endif

/// 真值
#ifndef TRUE
#define TRUE	1
#endif
/// 假值
#ifndef FALSE
#define FALSE	0
#endif


    /// API版本类型
    typedef char            TApiVersionType[API_VERSION_LEN];
	/// 机构号
	typedef char			TOrganizationNO[ORG_NO_LEN];
	/// IP
	typedef char			TIPADDR[IP_ADDR_LEN];
	//  端口
	typedef int				TPort;

    /// 错误代码类型
    typedef int             TErrorNoType;
    /// 错误信息类型
    typedef char            TErrorMsgType[MSG_LEN];

    /// 客户号类型
    typedef char            TCustNoType[CUST_NO_LEN];
	/// 股东帐号类型
    typedef char            THolderNoType[HOLDER_NO_LEN];
    /// 客户登录密码类型
    typedef char            TCustPwdType[PWD_LEN];
    /// 终端类型类型
    typedef int             TTerminalTypeType;
    /// 备注类型
    typedef char            TNoteType[NOTE_LEN];
	/// 短摘要类型
    typedef char            TNoteType2[NOTE_LEN2];
    /// 柜台实例号类型
    typedef char            TInstanceIdType[INSTANCE_ID_LEN];
    /// 网卡地址类型
    typedef char            TMacType[MAC_ADDR_LEN];
    
    /// 会话号类型
    typedef int             TSessionIdType;
    /// 连接会话号类型
    typedef int             TConnSessionIdType;
    /// 客户姓名类型
    typedef char            TCustNameType[CUST_NAME_LEN];
    /// 交易日期类型
    typedef char            TDateType[DATE_TYPE_LEN];


    /// 市场代码类型，具体值含义见数据字典-市场代码
    typedef char            TMarketCodeType;
    /// 证券代码类型
    typedef char            TSecCodeType[SEC_CODE_LEN];
    /// 委托类型类型，具体值含义见数据字典-委托类型
    typedef char            TOrderTypeType;
    /// 买卖类型类型，具体值含义见数据字典-买卖类型
    typedef char            TBSType[BS_TYPE_LEN];
	/// 买卖标志类型
    typedef char            TBSFLAG;
    /// 市价单类型类型，具体值含义见数据字典-市价单类型
    typedef char            TMarketOrderFlagType;
    /// 委托价格类型
    typedef double          TPriceType;
    /// 委托数量类型
    typedef double          TStockVolType;
    /// 委托属性类型
    typedef int             TOrderPropType;
    /// 策略编号/批号类型
    typedef int             TBatchNoType;
    
    /// 委托属性类型
    typedef int             TOrderPropType;
    /// 委托流水号类型
    typedef char            TOrderNoType[ORDER_NO_LEN];
	/// 合同号类型
	typedef int				TContractNoType;
    /// 委托状态类型，具体值含义见数据字典-委托状态
    typedef char            TOrderStatusType;
    /// 委托状态名称
    typedef char            TOrderStatusName[ORDER_STATUS_NAME_LEN];
    /// 委托时间类型
    typedef int             TTimeType;
    /// 成交次数类型
    typedef int             TDoneCountType;
	//	撤单类型
	typedef int             TOrderCancelType;
    
    /// 成交编号类型
    typedef char            TDoneNoType[DONE_NO_LEN];
	/// 成交ID类型
	typedef int				TDoneIDType;
    
    /// 货币类型类型，具体值含义见数据字典-货币类型
    typedef char            TCurrencyTypeType;
    /// 资金数量类型
    typedef double          TFundAmountType;
    /// 查询账户资金类型
    typedef int             TQueryAccountProp;
    /// 查询持仓类型
    typedef int             TQueryPositionProp;
    
    /// 查询委托属性类型
    typedef int             TQueryOrderPropType;
    
	//	查询类型 增量 全量
	typedef int             TQueryType;
    /// 证券简称类型
    typedef char            TSecNameType[SEC_NAME_LEN];
    /// 证券状态类型
    typedef short           TSecStatusType;
    /// 证券品种，具体值含义见数据字典-证券品种
    typedef int             TSecVarietyType;
    /// ETF简称类型
    typedef char            TEtfNameType[ETF_NAME_LEN];
    /// 单位类型
    typedef int             TUnitType;
    /// 比例类型
    typedef double          TRatioType;
    /// 发布单位净参考值标志类型
    typedef char            TPublishIOPVFlag;
    /// ETF状态类型，具体值含义见数据字典-ETF交易状态
    typedef char            TEtfStatusType;
    /// 篮子股票个数类型
    typedef int             TRecordNumType;
    /// 现金替代标志类型，具体值含义见数据字典-现金替代标志
    typedef char            TCashReplFlagType;
	/// ETF属性，具体值含义见数据字典-ETF属性
    typedef char            TEtfPropType[ETF_PROP_LEN];

	/// 废单原因（ETX推送），废单有效，深圳65-88为WTCLBZ的ascii码，其他为HBCDYY，上海REMARK
	typedef char			TInvalidOrderReason[INVALID_ORDER_REASON_LEN];
	/// 业务功能号类型（ETX推送），16203-成交撤单废单数据推送，16204-撤单的废单数据推送
	typedef char			TAppFunctionNO[APP_FUNC_NO_LEN];

	/// 代理用户名
	typedef char			TProxyUser[PROXYUSER_LEN];
	/// 代理用户密码
	typedef char			TProxyPass[PROXYPASS_LEN];

/** @} */
#ifdef __cplusplus
}
#endif

#endif //__SPXAPI_TYPE_H__
