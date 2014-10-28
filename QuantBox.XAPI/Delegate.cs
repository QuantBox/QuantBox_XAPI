using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace QuantBox.XAPI
{
    public delegate void DelegateOnConnectionStatus(object sender,ConnectionStatus status,ref RspUserLoginField userLogin,int size1);
    public delegate void DelegateOnRtnError(object sender, ref ErrorField error);

    public delegate void DelegateOnRtnDepthMarketData(object sender,ref DepthMarketDataField marketData);
    public delegate void DelegateOnRtnQuoteRequest(object sender,ref QuoteRequestField quoteRequest);
    
    public delegate void DelegateOnRspQryInstrument(object sender, ref InstrumentField instrument,int size1, bool bIsLast);
    public delegate void DelegateOnRspQryTradingAccount(object sender, ref AccountField account, int size1, bool bIsLast);
    public delegate void DelegateOnRspQrySettlementInfo(object sender, ref SettlementInfoField settlementInfo, int size1, bool bIsLast);
    public delegate void DelegateOnRtnOrder(object sender, ref OrderField order);
    public delegate void DelegateOnRtnTrade(object sender, ref TradeField trade);
}
