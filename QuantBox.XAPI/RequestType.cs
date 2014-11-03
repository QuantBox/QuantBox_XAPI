using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace QuantBox.XAPI
{
    public enum RequestType:byte
    {
        GetApiType = 0,
        GetApiVersion,
        GetApiName,

        Create, // 创建
        Release, // 销毁
        Register, // 注册回调
        Config,		// 配置参数

        Connect, // 开始/连接
        Disconnect, // 停止/断开

        Clear, // 清理
        Process, // 处理

        Subscribe, // 订阅
        Unsubscribe, // 取消订阅

        SubscribeQuote, // 订阅应价
        UnsubscribeQuote, // 取消应价

        ReqOrderInsert,
        ReqQuoteInsert,
        ReqOrderAction,
        ReqQuoteAction,

        ReqQryOrder,
        ReqQryTrade,

        ReqQryInstrument,
        ReqQryTradingAccount,
        ReqQryInvestorPosition,
        ReqQryInvestorPositionDetail,
        ReqQryInstrumentCommissionRate,
        ReqQryInstrumentMarginRate,
        ReqQrySettlementInfo,
    }
}
