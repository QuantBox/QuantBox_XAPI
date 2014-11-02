using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace QuantBox
{
    /// <summary>
    /// 连接状态
    /// </summary>
    public enum ConnectionStatus : byte
    {
        /// <summary>
        /// 未初始化
        /// </summary>
        [Description("未初始化")]
        Uninitialized = 0,
        /// <summary>
        /// 已经初始化
        /// </summary>
        [Description("已经初始化")]
        Initialized,
        /// <summary>
        /// 未连接
        /// </summary>
        [Description("未连接")]
        Disconnected,
        /// <summary>
        /// 连接中...
        /// </summary>
        [Description("连接中...")]
        Connecting,
        /// <summary>
        /// 连接成功
        /// </summary>
        [Description("连接成功")]
        Connected,
        /// <summary>
        /// 授权中...
        /// </summary>
        [Description("授权中...")]
        Authorizing,
        /// <summary>
        /// 授权成功
        /// </summary>
        [Description("授权成功")]
        Authorized,
        /// <summary>
        /// 登录中...
        /// </summary>
        [Description("登录中...")]
        Logining,
        /// <summary>
        /// 登录成功
        /// </summary>
        [Description("登录成功")]
        Logined,
        /// <summary>
        /// 确认中...
        /// </summary>
        [Description("确认中...")]
        Confirming,
        /// <summary>
        /// 已经确认
        /// </summary>
        [Description("已经确认")]
        Confirmed,
        /// <summary>
        /// 已经确认
        /// </summary>
        [Description("操作中...")]
        Doing,
        /// <summary>
        /// 完成
        /// </summary>
        [Description("完成")]
        Done,
        /// <summary>
        /// 未知
        /// </summary>
        [Description("未知")]
        Unknown,
    }

    public enum ApiType : byte
    {
        Nono = 0,
	    Trade = 1,
	    MarketData = 2,
	    Level2 = 4,
    };

    public enum DataLevelType : byte
    {
        L0,
	    L1,
	    L5,
	    L10,
	    FULL,
    };
    public enum ResumeType : byte
    {
        Restart,
        Resume,
        Quick,
    };

    public enum PutCall : byte
    {
        Put,
        Call,
    };

    public enum OrderStatus : byte
    {
        NotSent,
        PendingNew,
        New,
        Rejected,
        PartiallyFilled,
        Filled,
        PendingCancel,
        Cancelled,
        Expired,
        PendingReplace,
        Replaced,
    };

    public enum OrderSide : byte
    {
        Buy,
        Sell,
    };

    public enum OrderType : byte
    {
        Market,
        Stop,
        Limit,
        StopLimit,
        MarketOnClose,
        TrailingStop,
        TrailingStopLimit,
    };

    public enum TimeInForce : byte
    {
        ATC,
        Day,
        GTC,
        IOC,
        OPG,
        OC,
        FOK,
        GTX,
        GTD,
        GFS,
    };

    public enum PositionSide : byte
    {
        Long,
        Short,
    };

    public enum ExecType : byte
    {
        ExecNew,
        ExecRejected,
        ExecTrade,
        ExecPendingCancel,
        ExecCancelled,
        ExecCancelReject,
        ExecPendingReplace,
        ExecReplace,
        ExecReplaceReject,
    };

    public enum OpenCloseType : byte
    {
        Open,
        Close,
        CloseToday,
    };

    public enum HedgeFlagType : byte
    {
        Speculation,
        Arbitrage,
        Hedge,
        MarketMaker,
    };

    public enum InstrumentType : byte
    {
	    Stock,
	    Future,
	    Option,
	    FutureOption,
	    Bond,
	    FX,
	    Index,
	    ETF,
	    MultiLeg,
	    Synthetic,
    };
}
