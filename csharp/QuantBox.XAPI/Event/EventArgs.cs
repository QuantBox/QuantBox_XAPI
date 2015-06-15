using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace QuantBox.XAPI.Event
{
    public class OnConnectionStatusEventArgs:EventArgs
    {
        public readonly ConnectionStatus status;
        public readonly RspUserLoginField userLogin;
        public readonly int size1;
        
        public OnConnectionStatusEventArgs(ConnectionStatus status, ref RspUserLoginField userLogin, int size1)
        {
            this.status = status;
            this.userLogin = userLogin;
            this.size1 = size1;
        }
    }

    public class OnRtnErrorEventArgs : EventArgs
    {
        public readonly ErrorField error;

        public OnRtnErrorEventArgs(ref ErrorField error)
        {
            this.error = error;
        }
    }

    public class OnRtnDepthMarketDataNEventArgs : EventArgs
    {
        public readonly DepthMarketDataNClass marketData;

        public OnRtnDepthMarketDataNEventArgs(ref DepthMarketDataNClass marketData)
        {
            this.marketData = marketData;
        }
    }

    public class OnRtnQuoteRequestEventArgs : EventArgs
    {
        public readonly QuoteRequestField quoteRequest;

        public OnRtnQuoteRequestEventArgs(ref QuoteRequestField quoteRequest)
        {
            this.quoteRequest = quoteRequest;
        }
    }

    public class OnRspQryInstrumentEventArgs : EventArgs
    {
        public readonly InstrumentField instrument;
        public readonly int size1;
        public readonly bool bIsLast;

        public OnRspQryInstrumentEventArgs(ref InstrumentField instrument, int size1, bool bIsLast)
        {
            this.instrument = instrument;
            this.size1 = size1;
            this.bIsLast = bIsLast;
        }
    }

    public class OnRspQryTradingAccountEventArgs : EventArgs
    {
        public readonly AccountField account;
        public readonly int size1;
        public readonly bool bIsLast;

        public OnRspQryTradingAccountEventArgs(ref AccountField account, int size1, bool bIsLast)
        {
            this.account = account;
            this.size1 = size1;
            this.bIsLast = bIsLast;
        }
    }

    public class OnRspQryInvestorPositionEventArgs : EventArgs
    {
        public readonly PositionField position;
        public readonly int size1;
        public readonly bool bIsLast;

        public OnRspQryInvestorPositionEventArgs(ref PositionField position, int size1, bool bIsLast)
        {
            this.position = position;
            this.size1 = size1;
            this.bIsLast = bIsLast;
        }
    }

    public class OnRspQrySettlementInfoEventArgs : EventArgs
    {
        public readonly SettlementInfoField settlementInfo;
        public readonly int size1;
        public readonly bool bIsLast;

        public OnRspQrySettlementInfoEventArgs(ref SettlementInfoField settlementInfo, int size1, bool bIsLast)
        {
            this.settlementInfo = settlementInfo;
            this.size1 = size1;
            this.bIsLast = bIsLast;
        }
    }

    public class OnRtnOrderEventArgs : EventArgs
    {
        public readonly OrderField order;

        public OnRtnOrderEventArgs(ref OrderField order)
        {
            this.order = order;
        }
    }

    public class OnRtnTradeEventArgs : EventArgs
    {
        public readonly TradeField trade;

        public OnRtnTradeEventArgs(ref TradeField trade)
        {
            this.trade = trade;
        }
    }

    public class OnRtnQuoteEventArgs : EventArgs
    {
        public readonly QuoteField quote;

        public OnRtnQuoteEventArgs(ref QuoteField quote)
        {
            this.quote = quote;
        }
    }

    public class OnRspQryHistoricalTicksEventArgs : EventArgs
    {
        public readonly IntPtr pTicks;
        public readonly int size1;
        public readonly HistoricalDataRequestField request;
        public readonly int size2;
        public readonly bool bIsLast;

        public OnRspQryHistoricalTicksEventArgs(IntPtr pTicks, int size1, ref HistoricalDataRequestField request, int size2, bool bIsLast)
        {
            this.pTicks = pTicks;
            this.size1 = size1;
            this.request = request;
            this.size2 = size2;
            this.bIsLast = bIsLast;
        }
    }

    public class OnRspQryHistoricalBarsEventArgs : EventArgs
    {
        public readonly IntPtr pTicks;
        public readonly int size1;
        public readonly HistoricalDataRequestField request;
        public readonly int size2;
        public readonly bool bIsLast;

        public OnRspQryHistoricalBarsEventArgs(IntPtr pTicks, int size1, ref HistoricalDataRequestField request, int size2, bool bIsLast)
        {
            this.pTicks = pTicks;
            this.size1 = size1;
            this.request = request;
            this.size2 = size2;
            this.bIsLast = bIsLast;
        }
    }

    public class OnRspQryInvestorEventArgs : EventArgs
    {
        public readonly InvestorField investor;
        public readonly int size1;
        public readonly bool bIsLast;

        public OnRspQryInvestorEventArgs(ref InvestorField investor, int size1, bool bIsLast)
        {
            this.investor = investor;
            this.size1 = size1;
            this.bIsLast = bIsLast;
        }
    }
}
