
/****************************************************************************************
*									免费数据接口规范 V1.0    							*
*									     2010.11										*
*****************************************************************************************/

/****************************************************************************************
*					在注册库的 HKEY_LOCAL_MACHINE/SOFTWARE/StockDrv 下					*
*					Driver  = "驱动DLL全路径"											*
*					Provider= "TongShi"													*
*																						*
*	建议采用 LoadLibrary 和 GetProcAddress 及 FreeLibrary 的方式调用股票接收驱动		*
*	股票接收驱动的文件名及存放位置由 注册库 HKEY_LOCAL_MACHINE/Software/Stockdrv		*
*	的 Driver 指定																		*
*																						*
*																						*
*	==========================		按字节对齐编译		===========================		*
*																						*
*																						*
****************************************************************************************/


#ifndef __STOCKDRV_H__
#define __STOCKDRV_H__

// 工作方式类型定义
#define RCV_WORK_REQUEST			0			// 如果自编软件建议使用此方式
#define RCV_WORK_SENDMSG			4			// 版本 2 建议使用的方式


//	消息类型
#define RCV_REPORT			0x3f001234
#define RCV_FILEDATA		0x3f001235


#define	TS_StockDrv_IdTxt "TongShi_StockDrv_2.00"	// 通视信息卡特征串


// 证券市场
#define SH_MARKET_EX				'HS'		// 上海
#define SZ_MARKET_EX				'ZS'		// 深圳
#define HK_MARKET_EX			    'KH'		// 香港
#define EB_MARKET_EX			    'BE'		// 二板
#define SB_MARKET_EX			    'BS'		// 三板


// 文件数据类型
// 结构数组形式的文件数据

#define FILE_HISTORY_EX				2			// 补历史数据
#define FILE_MINUTE_EX				4			// 补分时数据
#define FILE_POWER_EX				6			// 补除权信息
#define FILE_TYPE_RES				-1			// 保留


#define RI_IDSTRING					1			// 厂商名称，返回(LPCSTR)厂商名
#define RI_IDCODE					2			// 卡号
#define RI_VERSION					3			// 驱动程序版本
#define RI_ERRRATE					4			// 取信道误码
#define RI_STKNUM					5			// 取上市股票总家数
#define RI_V2SUPPORT				6			// 支持深圳SJS库结构

#define STKLABEL_LEN				10			// 股号数据长度,国内市场股号编码兼容钱龙
#define STKNAME_LEN					32			// 股名长度



#pragma pack(1)


///////////////////////////////////////////////////////////////////////////////
// 行情数据
// 注：
//    某些字段可能为0，用上次的数据代替
typedef struct tagRCV_REPORT_STRUCTEx
{
	WORD	m_cbSize;									// 结构大小
	time_t	m_time;										// 交易时间
	WORD	m_wMarket;									// 股票市场类型
	char	m_szLabel[STKLABEL_LEN];					// 股票代码,以'\0'结尾
	char	m_szName[STKNAME_LEN];						// 股票名称,以'\0'结尾

	float	m_fLastClose;								// 昨收
	float	m_fOpen;									// 今开
	float	m_fHigh;									// 最高
	float	m_fLow;										// 最低
	float	m_fNewPrice;								// 最新
	float	m_fVolume;									// 成交量,以股为单位,股的含义依股票类型定义
	float	m_fAmount;									// 成交额,以元为单位

	float	m_fBuyPrice[3];								// 申买价1,2,3
	float	m_fBuyVolume[3];							// 申买量1,2,3
	float	m_fSellPrice[3];							// 申卖价1,2,3
	float	m_fSellVolume[3];							// 申卖量1,2,3

	float	m_fBuyPrice4;								// 申买价4
	float	m_fBuyVolume4;								// 申买量4
	float	m_fSellPrice4;								// 申卖价4
	float	m_fSellVolume4;								// 申卖量4
	float	m_fBuyPrice5;								// 申买价5
	float	m_fBuyVolume5;								// 申买量5
	float	m_fSellPrice5;								// 申卖价5
	float	m_fSellVolume5;								// 申卖量5
} RCV_REPORT_STRUCTEx;


//////////////////////////////////////////////////////////////////////////////////
//补充数据头
//数据头 m_dwHeadTag == EKE_HEAD_TAG 
#define EKE_HEAD_TAG	0xffffffff

typedef struct tagRCV_EKE_HEADEx
{
	DWORD	m_dwHeadTag;								// = EKE_HEAD_TAG
	WORD	m_wMarket;									// 市场类型
	char	m_szLabel[STKLABEL_LEN];					// 股票代码
} RCV_EKE_HEADEx;


// 历史数据
// 注一:
//		每一数据结构都应通过 m_time == EKE_HEAD_TAG, 判断是否为 m_head, 然后再作解释
typedef union tagRCV_HISTORY_STRUCTEx
{
	struct
	{
		time_t	m_time;				// UCT
		float	m_fOpen;			// 开盘
		float	m_fHigh;			// 最高
		float	m_fLow;				// 最低
		float	m_fClose;			// 收盘
		float	m_fVolume;			// 量
		float	m_fAmount;			// 额
		union
		{
			struct
			{
				WORD m_wAdvance;	// 涨数, 仅大盘有效
				WORD m_wDecline;	// 跌数, 仅大盘有效
			};
			float m_fActiveBuyVol;	// 主动买量
		};
	};
	RCV_EKE_HEADEx m_head;
} RCV_HISTORY_STRUCTEx;


// 分时数据
// 注一:
//		每一数据结构都应通过 m_time == EKE_HEAD_TAG, 判断是否为 m_head, 然后再作解释
typedef union tagRCV_MINUTE_STRUCTEx
{
	struct
	{
		time_t	m_time;				// UCT
		float	m_fPrice;			// 价
		float	m_fVolume;			// 量
		float	m_fAmount;			// 额
	};
	RCV_EKE_HEADEx m_head; 
} RCV_MINUTE_STRUCTEx;


// 除权数据
// 注一:
//		每一数据结构都应通过 m_time == EKE_HEAD_TAG, 判断是否为 m_head, 然后再作解释
typedef union tagRCV_POWER_STRUCTEx
{
	struct
	{
		time_t	m_time;				// UCT
		float	m_fGive;			// 每股送
		float	m_fPei;				// 每股配
		float	m_fPeiPrice;		// 配股价, 仅当 m_fPei != 0.0f 时有效
		float	m_fProfit;			// 每股红利
	};
	RCV_EKE_HEADEx m_head;
} RCV_POWER_STRUCTEx;


//////////////////////////////////////////////////////////////////////////////////
// 文件类型数据包头
//  注一:
//	m_wDataType == FILE_BASE_EX
//				m_dwAttrib = 股票证券市场,m_szFileName仅包含文件名
//				m_FileTime = 基本面资料文件创建日期
//  m_wDataType == FILE_NEWS_EX
//				m_dwAttrib = 消息来源,m_szFileName 包含目录的文件名,目录名为消息来源
//				m_dwSerialNo = 序列号
//				如:  "上交所消息\\0501Z012.TXT","新兰德\\XLD0001.TXT"
//  注二:
//		数据文件循环播出,每个文件有唯一的序列号,以避免重复接收
typedef struct tagRCV_FILE_HEADEx
{
	DWORD	m_dwAttrib;							// 文件子类型
	DWORD	m_dwLen;							// 文件长度
	union
	{
		DWORD	m_dwSerialNo;					// 序列号,对股评
		time_t	m_FileTime;						// 文件创建时间
	};
	char	m_szFileName[_MAX_PATH];			// 文件名 or URL
} RCV_FILE_HEADEx;



//////////////////////////////////////////////////////////////////////////////////
// 数据通知消息
// wParam = RCV_WPARAM;
// lParam 指向 RCV_DATA结构;
// 返回 1 已经处理, 0 未处理或不能处理, 目前该返回值被忽略
// 注一:
//		记录数表示行情数据和补充数据(包括 Header)的数据包数, 对文件类型数据, = 1
// 注二:
//		若 m_bDISK = FALSE, m_pData 为消息股评等数据缓冲区指针
//
//		******** 数据共享, 不能修改数据 **********
//
//		m_bDISK = TRUE, m_pData 为该文件的存盘文件名, 一般情况只有
//		升级软件等大文件用存盘方式
typedef struct tagRCV_DATA
{
	int					m_wDataType;			// 文件类型
	int					m_nPacketNum;			// 记录数, 参见注一
	RCV_FILE_HEADEx		m_File;					// 文件接口
	BOOL				m_bDISK;				// 文件是否已存盘的文件
	union
	{
		RCV_REPORT_STRUCTEx* m_pReport;			// 行情数据
		RCV_HISTORY_STRUCTEx* m_pDay;			// 补日线数据
		RCV_MINUTE_STRUCTEx* m_pMinute;			// 补分时数据
		RCV_POWER_STRUCTEx* m_pPower;			// 补除权数据
		void* m_pData;							// 内存指针
	};
} RCV_DATA;


/* 消息处理程序 DEMO
LONG OnStkDataOK(UINT wParam, LONG lParam)
{
	RCV_DATA* pHeader;
	int i;

	pHeader = (RCV_DATA*)lParam;

	//	对于处理速度慢的数据类型,最好将 pHeader->m_pData 指向的数据备份, 再作处理
	switch (wParam)
	{
	case RCV_REPORT:						// 股票行情
		for (i = 0; i < pHeader->m_nPacketNum; i++)
		{
			pHeader->m_pReport[i] ...
			// 数据处理
		}
		break;

	case RCV_FILEDATA:						// 文件
		switch (pHeader->m_wDataType)
		{
		case FILE_HISTORY_EX:				// 补日线数据
			break;

		case FILE_MINUTE_EX:				// 补分钟线数据
			break;

		case FILE_BASE_EX:					// 钱龙兼容基本资料文件, m_szFileName仅包含文件名
			break;

		case FILE_NEWS_EX:					// 新闻类, 其类型由m_szFileName中子目录名来定
			break;

		default:
			return 0;
		}
		break;

	default:
		return 0;							// unknown data
	}

	return 1;
}
*/


// ****************************************************************************
// 新版消息类型，非通视方式，适用于自编软件
enum STKMessageType
{
	stkMessageLoginAuth			= 0x1000,		// 登录状态
	stkMessageStkLabel			= 0x1001,		// 证券代码表
	stkMessageReport			= 0x1234,		// 行情数据
	stkMessageTick				= 0x2001,		// 分笔(盘口数据)
	stkMessagePower				= 0x2002,		// 除权
	stkMessageFinance			= 0x2003,		// 财务
	stkMessageMinute1			= 0x3001,		// 一分钟
	stkMessageMinute5			= 0x3005,		// 五分钟
	stkMessageHistory			= 0x3002,		// 日线
};

// ****************************************************************************
// 登录状态，接口登录成功会收到 stkMessageLoginAuth 类型的消息
// bAuthorizationState = TRUE，表示登录成功
typedef struct tagSTK_LOGINAUTH_STRUCTEx
{
	BOOL bAuthorizationState;
} STK_LOGINAUTH_STRUCTEx;

// ****************************************************************************
// 证券代码表
typedef  struct  tagSTK_STKLABEL_STRUCTEx
{
	WORD m_wMarket;								// 股票市场类型
	char m_szLabel[STKLABEL_LEN];				// 股票代码
	char m_szName[STKNAME_LEN];					// 股票名称
	char m_szPingYing[STKLABEL_LEN];			// 股票名称拼音
	float m_fLastClose;							// 前一交易日收盘价
	time_t m_time;								// 最新名称变更时间
} STK_STKLABEL_STRUCTEx;

// ****************************************************************************
// 分笔数据结构
typedef union tagSTK_TICK_STRUCTEx
{
	struct
	{
		time_t	m_time;							// UCT
		float	m_fNewPrice;					// 最新价
		float	m_fVolume;						// 成交量
		float	m_fAmount;						// 成交额
		float	m_Price[9];						// 未使用
	};
	RCV_EKE_HEADEx m_head;
} STK_TICK_STRUCTEx;

// ****************************************************************************
// 数据头
typedef struct tagSTK_EKE_HEADEx
{
	DWORD	m_dwHeadTag;						// = EKE_HEAD_TAG
	WORD	m_wFlag;							// 未使用 00...
	WORD	m_wMarket;							// 市场类型
	char	m_szLabel[STKLABEL_LEN];			// 股票代码
} STK_EKE_HEADEx;

// ****************************************************************************
// 财务数据结构
typedef union tagSTK_FINANCE_STRUCT
{
	struct
	{
		time_t time;		// 更新时间

		float m_fZgb;		// 总股本
		float m_fGjg;		// 国家股
		float m_fFqrf;		// 发起法人股**
		float m_fFrg;		// 法人股**
		float m_fBg;		// B股
		float m_fHg;		// H股
		float m_fAg;		// 流通A股
		float m_fZgg;		// 职工股**
		float m_fA2zp;		// A2转配股**
		float m_fZzc;		// 总资产（千元）
		float m_fLdzc;		// 流动资产
		float m_fGdzc;		// 固定资产
		float m_fWxzc;		// 无形资产
		float m_fCqzc;		// 长期资产
		float m_fLdfz;		// 流动负债
		float m_fCqfz;		// 长期负债
		float m_fZbgjj;		// 资本公积金
		float m_fMggjj;		// 每股公积金
		float m_fGdqy;		// 股东权益
		float m_fZysr;		// 主营收入
		float m_fZylr;		// 主营利润
		float m_fQtlr;		// 其他利润
		float m_fYylr;		// 营业利润
		float m_fTzsy;		// 投资收益
		float m_fBtsr;		// 补贴收入
		float m_fYywsr;		// 营业外收支
		float m_fSytz;		// 损益调整
		float m_fLrze;		// 利润总额
		float m_fYszkl;		// 应收帐款周转率**
		float m_fJly;		// 净利润
		float m_fWfply;		// 未分配利润
		float m_fMgwfp;		// 每股未分配
		float m_fMgsy;		// 每股收益
		float m_fMgjzc;		// 每股净资产
		float m_fTzmgjzc;	// 调整每股净资**
		float m_fGdqyb;		// 股东权益比率**
		float m_fJzcsyl;	// 净资收益率
		float m_fJyxjlc;	// 经营现金流出
		float m_fJyxjlr;	// 经营现金注入
		float m_fJyxjll;	// 经营现金流量
		float m_fTzxjlr;	// 投资现金流入
		float m_fTzxjlc;	// 投资现金流出
		float m_fTzxjll;	// 投资现金流量
		float m_fCzxjlr;	// 筹资现金流入
		float m_fCzxjlc;	// 筹资现金流出
		float m_fCzxjll;	// 筹资现金流量
		float m_fXjdjw;		// 现金及等价物
		float m_fYszzzl;	// 应收帐周转率
		float m_fChzzl;		// 存货周转率
		float m_fGdzs;		// 股东总数
		float m_fFxj;		// 发行价
		float m_fFxl;		// 发行量
		float m_fZyywl;		// 主营业务增长率
		float m_fShlyl;		// 税后利润增长率
		float m_fJzcl;		// 净资产增长率
		float m_fZzcl;		// 总资产增长率
	};
	STK_EKE_HEADEx m_head;	// 
} STK_FINANCE_STRUCTx;

typedef struct tagSTK_DATA
{
	WORD							m_wDataType;	// 数据消息类型
	int								m_nPacketNum;	// 记录条数
	union
	{
		STK_LOGINAUTH_STRUCTEx*		m_pLoginAuth;
		STK_STKLABEL_STRUCTEx*		m_pStkLabel;
		RCV_REPORT_STRUCTEx*		m_pReport;
		STK_TICK_STRUCTEx*			m_pTick;
		RCV_POWER_STRUCTEx*			m_pPower;
		STK_FINANCE_STRUCTx*		m_pFinance;
		RCV_HISTORY_STRUCTEx*		m_pMinute1;
		RCV_HISTORY_STRUCTEx*		m_pMinute5;
		RCV_HISTORY_STRUCTEx*		m_pHistory;
		void*						m_pData;
	};
} STK_DATA;

#pragma pack()



///////////////////////////////////////////////////////////////////////////////
// APIs

#ifdef __cplusplus
extern "C"
{
#endif

///////////////////////////////////////////////////////////////////////////////
// 注册函数

// 股票初始化
// 入口参数:
//		hWnd			处理消息的窗口句柄
//		Msg				用户自定义消息
//		nWorkMode		接口工作方式，如果自编软件使用请求的方式 RCV_WORK_REQUEST，注意消息数据类型
//                      要按 STKMessageType 定义的值接收
// 返回参数:
//		 1				成功
//		-1				失败
// 注:
//		注册后,驱动程序会向处理窗口发送消息; 若不注册,通过查询方式亦可取得数据
//		若股票接收没启动,则启动股票接收程序
int WINAPI Stock_Init(HWND hWnd, UINT Msg, int nWorkMode);

// 退出,停止发送消息
// 入口参数:
//		hWnd			处理消息的窗口句柄,同 Stock_Init 的调用入口参数
//	返回参数:
//		 1				成功
//		-1				失败
int WINAPI Stock_Quit(HWND hWnd);

///////////////////////////////////////////////////////////////////////////////
// 行情接口

// 取已接收到的股票总数
int WINAPI GetTotalNumber();

// 由序号取股票数据(扩展)
// 入口参数:
//			nNo			序号
//			pBuf		存放股票数据的缓冲区
// 返回参数:
//		    NoStockData	无股票数据
// 注:
//			该函数提供股票数据的主要数据;分析软件刚运行时,可以快速建立数据框架
int WINAPI GetStockByNoEx(int nNo, RCV_REPORT_STRUCTEx* pBuf);

// 由股号取股票数据(扩展)
// 入口参数:	
//			pszStockCode股票代号
//			nMarket		证券市场
//			pBuf		存放股票数据的缓冲区
// 返回参数:
//		    NoStockData	无股票数据
// 注:
//			该函数提供股票数据的主要数据;分析软件刚运行时,可以快速建立数据框架
int WINAPI GetStockByCodeEx(char* pszStockCode, int nMarket, RCV_REPORT_STRUCTEx* pBuf);

// 激活接收程序,进行设置
// 入口参数:
//			bSetup		TRUE		显示窗口,进行设置
// 返回参数:
//			 1			成功
//			-1			失败
int	WINAPI SetupReceiver(BOOL bSetup);

//	取得股票驱动信息
//	入口参数:
//			nInfo		索引
//			pBuf		缓冲区
//	出口参数:
//			nInfo == RI_IDSTRING,	返回特征字符串长度, pBuf 为特征字符串
//									如:	"TongShi_StockDrv_1.00"
//			nInfo == RI_IDCODE,		返回信息卡 ID 号, pBuf 为字符串形式的 ID 号
//									如:	0x78001234	  "78001234"
//			nInfo == RI_VERSION,	返回信息卡版本号, pBuf 为字符串版本
//									如:  1.00		  "1.00"
//			nInfo == RI_ERRRATE,	取信道误码,
//			nInfo == RI_STKNUM,		取上市股票总家数
DWORD WINAPI GetStockDrvInfo(int nInfo, void* pBuf);


// 请求股票数据类型
enum
{
	stkRequestTick		= 0x00,		// 盘口分笔，请求盘口会同时发送请求股票除权数据和财务数据
	stkRequestMin1		= 0x01,		// 一分钟
	stkRequestMin5		= 0x02,		// 五分钟
	stkRequestDay		= 0x05,		// 日线
};

//  请求股票数据
//  入口参数:
//          wMarket     股票市场 'HS' 'ZS' ....
//          szCode      股票代码 '600001' ....
//          nType       请求的数据类型
BOOL WINAPI RequestData(WORD wMarket, LPTSTR szCode, int nType);


#ifdef __cplusplus
}
#endif


#endif // __STOCKDRV_H__
