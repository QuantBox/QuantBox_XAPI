using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace QuantBox.XAPI
{
    public enum ResponeType : byte
    {
        OnConnectionStatus = 64,
        OnRtnDepthMarketData,
        OnRtnError,
        OnRspQryInstrument,
        OnRspQryTradingAccount,
        OnRspQrySettlementInfo,
        OnRtnOrder,
        OnRtnTrade,
        OnRtnQuoteRequest,
    }
}
