using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;

namespace QuantBox.XAPI
{
    /// <summary>
    /// 用户信息
    /// </summary>
    [StructLayout(LayoutKind.Sequential, CharSet = CharSet.Ansi)]
    public struct UserInfoField
    {
        /// <summary>
        /// 用户代码
        /// </summary>
        [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 32)]
        public string UserID;
        /// <summary>
        /// 密码
        /// </summary>
        [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 41)]
        public string Password;
    }

    /// <summary>
    /// 用户信息
    /// </summary>
    [StructLayout(LayoutKind.Sequential, CharSet = CharSet.Ansi)]
    public struct ServerInfoField
    {
        /// <summary>
        /// 订阅主题
        /// </summary>
        [MarshalAs(UnmanagedType.U1)]
        public bool IsUsingUdp;
        /// <summary>
        /// 订阅主题
        /// </summary>
        [MarshalAs(UnmanagedType.U1)]
        public bool IsMulticast;
        /// <summary>
        /// 订阅主题
        /// </summary>
        public int TopicId;
        /// <summary>
        /// 端口号
        /// </summary>
        public int Port;
        /// <summary>
        /// 流恢复
        /// </summary>
        public ResumeType MarketDataTopicResumeType;
        public ResumeType PrivateTopicResumeType; 
        public ResumeType PublicTopicResumeType;        
        public ResumeType UserTopicResumeType;
        /// <summary>
        /// 经纪公司代码
        /// </summary>
        [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 11)]
        public string BrokerID;
        /// <summary>
        /// 用户端产品信息
        /// </summary>
        [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 11)]
        public string UserProductInfo;
        /// <summary>
        /// 认证码
        /// </summary>
        [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 17)]
        public string AuthCode;
        /// <summary>
        /// 地址
        /// </summary>
        [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 512)]
        public string Address;
    }

    /// <summary>
    /// 深度行情
    /// </summary>
    [StructLayout(LayoutKind.Sequential, CharSet = CharSet.Ansi)]
    public struct ErrorField
    {
        /// <summary>
        /// 错误代码
        /// </summary>
        public int ErrorID;
        /// <summary>
        /// 错误信息
        /// </summary>
        [MarshalAs(UnmanagedType.ByValArray, SizeConst = 256)]
        public byte[] ErrorMsg;
    }

    /// <summary>
    /// 登录回报
    /// </summary>
    [StructLayout(LayoutKind.Sequential, CharSet = CharSet.Ansi)]
    public struct RspUserLoginField
    {
        /// <summary>
        /// 交易日
        /// </summary>
        [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 9)]
        public string TradingDay;
        /// <summary>
        /// 时间
        /// </summary>
        [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 9)]
        public string LoginTime;
        /// <summary>
        /// 错误信息
        /// </summary>
        [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 32)]
        public string SessionID;
        /// <summary>
        /// 错误代码
        /// </summary>
        public int ErrorID;
        /// <summary>
        /// 错误信息
        /// </summary>
        [MarshalAs(UnmanagedType.ByValArray, SizeConst = 256)]
        public byte[] ErrorMsg;
    }

    /// <summary>
    /// 深度行情
    /// </summary>
    [StructLayout(LayoutKind.Sequential, CharSet = CharSet.Ansi)]
    public struct DepthMarketDataField
    {
        public int TradingDay;
        public int ActionDay;
        public int UpdateTime;
        public int UpdateMillisec;
        /// <summary>
        /// 合约代码
        /// </summary>
        [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 64)]
        public string Symbol;
        /// <summary>
        /// 合约代码
        /// </summary>
        [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 31)]
        public string InstrumentID;
        /// <summary>
        /// 交易所代码
        /// </summary>
        [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 9)]
        public string ExchangeID;


        /// <summary>
        /// 最新价
        /// </summary>
        public double LastPrice;
        /// <summary>
        /// 数量
        /// </summary>
        public double Volume;
        /// <summary>
        /// 成交金额
        /// </summary>
        public double Turnover;
        /// <summary>
        /// 持仓量
        /// </summary>
        public double OpenInterest;
        /// <summary>
        /// 当日均价
        /// </summary>
        public double AveragePrice;


        /// <summary>
        /// 今开盘
        /// </summary>
        public double OpenPrice;
        /// <summary>
        /// 最高价
        /// </summary>
        public double HighestPrice;
        /// <summary>
        /// 最低价
        /// </summary>
        public double LowestPrice;
        /// <summary>
        /// 今收盘
        /// </summary>
        public double ClosePrice;
        /// <summary>
        /// 本次结算价
        /// </summary>
        public double SettlementPrice;


        /// <summary>
        /// 涨停板价
        /// </summary>
        public double UpperLimitPrice;
        /// <summary>
        /// 跌停板价
        /// </summary>
        public double LowerLimitPrice;
        /// <summary>
        /// 昨收盘
        /// </summary>
        public double PreClosePrice;
        /// <summary>
        /// 上次结算价
        /// </summary>
        public double PreSettlementPrice;
        /// <summary>
        /// 昨持仓量
        /// </summary>
        public double PreOpenInterest;
        

        /// <summary>
        /// 申买价一
        /// </summary>
        public double BidPrice1;
        /// <summary>
        /// 申买量一
        /// </summary>
        public int BidVolume1;
        /// <summary>
        /// 申卖价一
        /// </summary>
        public double AskPrice1;
        /// <summary>
        /// 申卖量一
        /// </summary>
        public int AskVolume1;
        /// <summary>
        /// 申买价二
        /// </summary>
        public double BidPrice2;
        /// <summary>
        /// 申买量二
        /// </summary>
        public int BidVolume2;
        /// <summary>
        /// 申卖价二
        /// </summary>
        public double AskPrice2;
        /// <summary>
        /// 申卖量二
        /// </summary>
        public int AskVolume2;
        /// <summary>
        /// 申买价三
        /// </summary>
        public double BidPrice3;
        /// <summary>
        /// 申买量三
        /// </summary>
        public int BidVolume3;
        /// <summary>
        /// 申卖价三
        /// </summary>
        public double AskPrice3;
        /// <summary>
        /// 申卖量三
        /// </summary>
        public int AskVolume3;
        /// <summary>
        /// 申买价四
        /// </summary>
        public double BidPrice4;
        /// <summary>
        /// 申买量四
        /// </summary>
        public int BidVolume4;
        /// <summary>
        /// 申卖价四
        /// </summary>
        public double AskPrice4;
        /// <summary>
        /// 申卖量四
        /// </summary>
        public int AskVolume4;
        /// <summary>
        /// 申买价五
        /// </summary>
        public double BidPrice5;
        /// <summary>
        /// 申买量五
        /// </summary>
        public int BidVolume5;
        /// <summary>
        /// 申卖价五
        /// </summary>
        public double AskPrice5;
        /// <summary>
        /// 申卖量五
        /// </summary>
        public int AskVolume5;
    }

    /// <summary>
    /// Tick行情
    /// </summary>
    [StructLayout(LayoutKind.Sequential, CharSet = CharSet.Ansi)]
    public struct TickField
    {
        public int Date;
        public int Time;
        public int Millisecond;

        public double LastPrice;
        public double Volume;
        public double OpenInterest;
        public double BidPrice1;
        public double AskPrice1;
        public int BidSize1;
        public int AskSize1;
    }

    /// <summary>
    /// Bar行情
    /// </summary>
    [StructLayout(LayoutKind.Sequential, CharSet = CharSet.Ansi)]
    public struct BarField
    {
        public int Date;
        public int Time;

        public double Open;
        public double High;
        public double Low;
        public double Close;
        public double Volume;
        public double OpenInterest;
        public double Turnover;
    }

    /// <summary>
    /// 发给做市商的询价请求
    /// </summary>
    [StructLayout(LayoutKind.Sequential, CharSet = CharSet.Ansi)]
    public struct QuoteRequestField
    {
        /// <summary>
        /// 合约代码
        /// </summary>
        [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 64)]
        public string Symbol;
        /// <summary>
        /// 合约代码
        /// </summary>
        [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 31)]
        public string InstrumentID;
        /// <summary>
        /// 交易所代码
        /// </summary>
        [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 9)]
        public string ExchangeID;
        /// <summary>
        /// 交易日
        /// </summary>
        [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 9)]
        public string TradingDay;
        /// <summary>
        /// 询价编号
        /// </summary>
        [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 64)]
        public string QuoteID;
        /// <summary>
        /// 询价时间
        /// </summary>
        [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 9)]
        public string QuoteTime;
    }

    /// <summary>
    /// 合约信息
    /// </summary>
    [StructLayout(LayoutKind.Sequential, CharSet = CharSet.Ansi)]
    public struct InstrumentField
    {
        /// <summary>
        /// 合约代码
        /// </summary>
        [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 64)]
        public string Symbol;
        /// <summary>
        /// 合约代码
        /// </summary>
        [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 31)]
        public string InstrumentID;
        /// <summary>
        /// 交易所代码
        /// </summary>
        [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 9)]
        public string ExchangeID;
        /// <summary>
        /// 合约名称
        /// </summary>
        [MarshalAs(UnmanagedType.ByValArray, SizeConst = 21)]
        public byte[] InstrumentName;
        /// <summary>
        /// 类型
        /// </summary>
        public InstrumentType_ Type;
        /// <summary>
        /// 合约数量乘数
        /// </summary>
        public int VolumeMultiple;
        /// <summary>
        /// 最小变动价位
        /// </summary>
        public double PriceTick;
        /// <summary>
        /// 到期日
        /// </summary>
        [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 9)]
        public string ExpireDate;
        /// <summary>
        /// 基础商品代码
        /// </summary>
        [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 31)]
        public string UnderlyingInstrID;
        /// <summary>
        /// 执行价
        /// </summary>
        public double StrikePrice;
        /// <summary>
        /// 期权类型
        /// </summary>
        public PutCall_ OptionsType;
    }

    /// <summary>
    /// 账号
    /// </summary>
    [StructLayout(LayoutKind.Sequential, CharSet = CharSet.Ansi)]
    public struct AccountField
    {
        /// <summary>
        /// 上次结算准备金
        /// </summary>
        public double PreBalance;
        /// <summary>
        /// 当前保证金总额
        /// </summary>
        public double CurrMargin;
        /// <summary>
        /// 平仓盈亏
        /// </summary>
        public double CloseProfit;
        /// <summary>
        /// 持仓盈亏
        /// </summary>
        public double PositionProfit;
        /// <summary>
        /// 期货结算准备金
        /// </summary>
        public double Balance;
        /// <summary>
        /// 可用资金
        /// </summary>
        public double Available;
        /// <summary>
        /// 入金金额
        /// </summary>
        public double Deposit;
        /// <summary>
        /// 出金金额
        /// </summary>
        public double Withdraw;
        /// <summary>
        /// 冻结的过户费
        /// </summary>
        public double FrozenTransferFee;
        /// <summary>
        /// 冻结的印花税
        /// </summary>
        public double FrozenStampTax;
        /// <summary>
        /// 冻结的手续费
        /// </summary>
        public double FrozenCommission;
        /// <summary>
        /// 冻结的资金
        /// </summary>
        public double FrozenCash;
        /// <summary>
        /// 过户费
        /// </summary>
        public double TransferFee;
        /// <summary>
        /// 印花税
        /// </summary>
        public double StampTax;
        /// <summary>
        /// 手续费
        /// </summary>
        public double Commission;
        /// <summary>
        /// 资金差额
        /// </summary>
        public double CashIn;
    }

    /// <summary>
    /// 账号
    /// </summary>
    [StructLayout(LayoutKind.Sequential, CharSet = CharSet.Ansi)]
    public struct SettlementInfoField
    {
        /// <summary>
        /// 交易日
        /// </summary>
        [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 9)]
        public string TradingDay;
        /// <summary>
        /// 消息正文
        /// </summary>
        [MarshalAs(UnmanagedType.ByValArray, SizeConst = 501)]
        public byte[] Content;
    }

    /// <summary>
    /// 订单信息
    /// </summary>
    [StructLayout(LayoutKind.Sequential, CharSet = CharSet.Ansi)]
    public struct QuoteField
    {
        /// <summary>
        /// 合约代码
        /// </summary>
        [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 31)]
        public string InstrumentID;
        /// <summary>
        /// 合约代码
        /// </summary>
        [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 9)]
        public string ExchangeID;

        public double AskQty;
        public double AskPrice;
        public OpenCloseType AskOpenClose;
        public HedgeFlagType AskHedgeFlag;

        public double BidQty;
        public double BidPrice;
        public OpenCloseType BidOpenClose;
        public HedgeFlagType BidHedgeFlag;

        [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 64)]
        public string ID;
        [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 64)]
        public string AskID;
        [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 64)]
        public string BidID;
        [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 64)]
        public string AskOrderID;
        [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 64)]
        public string BidOrderID;
        public OrderStatus_ Status;
        public ExecType_ ExecType;
        /// <summary>
        /// 错误代码
        /// </summary>
        public int ErrorID;
        /// <summary>
        /// 消息正文
        /// </summary>
        [MarshalAs(UnmanagedType.ByValArray, SizeConst = 256)]
        public byte[] Text;
    }

    /// <summary>
    /// 订单信息
    /// </summary>
    [StructLayout(LayoutKind.Sequential, CharSet = CharSet.Ansi)]
    public struct OrderField
    {
        /// <summary>
        /// 合约代码
        /// </summary>
        [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 31)]
        public string InstrumentID;
        /// <summary>
        /// 合约代码
        /// </summary>
        [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 9)]
        public string ExchangeID;
        /// <summary>
        /// 订单类型
        /// </summary>
        public OrderType_ Type;
        /// <summary>
        /// 合约代码
        /// </summary>
        public OrderSide_ Side;
        public double Qty;
        public double Price;
        public OpenCloseType OpenClose;
        public HedgeFlagType HedgeFlag;
        public double StopPx;
        public TimeInForce_ TimeInForce;

        public OrderStatus_ Status;
        public ExecType_ ExecType; 
        public double LeavesQty;
        public double CumQty;
        public double AvgPx;
        /// <summary>
        /// 错误代码
        /// </summary>
        public int ErrorID;
        /// <summary>
        /// 消息正文
        /// </summary>
        [MarshalAs(UnmanagedType.ByValArray, SizeConst = 256)]
        public byte[] Text;

        [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 64)]
        public string ID;
        [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 64)]
        public string OrderID;
        public int DateTime;
    }

    /// <summary>
    /// 订单信息
    /// </summary>
    [StructLayout(LayoutKind.Sequential, CharSet = CharSet.Ansi)]
    public struct TradeField
    {
        /// <summary>
        /// 合约代码
        /// </summary>
        [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 31)]
        public string InstrumentID;
        /// <summary>
        /// 合约代码
        /// </summary>
        [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 9)]
        public string ExchangeID;
        /// <summary>
        /// 合约代码
        /// </summary>
        public OrderSide_ Side;
        public double Qty;
        public double Price;
        public OpenCloseType OpenClose;
        public HedgeFlagType HedgeFlag;
        public double Commission;
        [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 9)]
        public string Time;
        [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 64)]
        public string ID;
        [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 64)]
        public string TradeID;
    }

    [StructLayout(LayoutKind.Sequential, CharSet = CharSet.Ansi)]
    public struct PositionField
    {
        /// <summary>
        /// 合约代码
        /// </summary>
        [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 64)]
        public string Symbol;
        /// <summary>
        /// 合约代码
        /// </summary>
        [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 31)]
        public string InstrumentID;
        /// <summary>
        /// 交易所代码
        /// </summary>
        [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 9)]
        public string ExchangeID;
        /// <summary>
        /// 合约代码
        /// </summary>
        public PositionSide_ Side;
        public double Position;
        public double TdPosition;
        public double YdPosition;
        public HedgeFlagType HedgeFlag;
    }

    /// <summary>
    /// 合约信息
    /// </summary>
    [StructLayout(LayoutKind.Sequential, CharSet = CharSet.Ansi)]
    public struct HistoricalDataRequestField
    {
        /// <summary>
        /// 合约代码
        /// </summary>
        [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 64)]
        public string Symbol;
        /// <summary>
        /// 合约代码
        /// </summary>
        [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 31)]
        public string InstrumentID;
        /// <summary>
        /// 交易所代码
        /// </summary>
        [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 9)]
        public string ExchangeID;

        public int Date1;
        public int Date2;
        public int Time1;
        public int Time2;

        public DataObjetType_ DataType;
        public BarType_ BarType;
        public int BarSize;

        public int RequestId;
        public int CurrentDate;
        public int lRequest;
    }
}
