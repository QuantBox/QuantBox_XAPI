using QuantBox.XAPI.Callback;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace QuantBox.XAPI.Event
{
    [ProgId("QuantBox.XAPI.Event.XApiWrapper"), ComVisible(true)]
    public class XApiWrapper:XApi
    {
        public new event EventHandler OnConnectionStatus;
        public new event EventHandler OnRtnError;

        public new event EventHandler OnRtnDepthMarketData;
        public new event EventHandler OnRtnQuoteRequest;

        public new event EventHandler OnRspQryInstrument;
        public new event EventHandler OnRspQryTradingAccount;
        public new event EventHandler OnRspQryInvestorPosition;
        public new event EventHandler OnRspQrySettlementInfo;
        public new event EventHandler OnRtnOrder;
        public new event EventHandler OnRtnTrade;
        public new event EventHandler OnRtnQuote;

        public new event EventHandler OnRspQryHistoricalTicks;
        public new event EventHandler OnRspQryHistoricalBars;

        public new event EventHandler OnRspQryInvestor;


        public XApiWrapper(string path):this()
        {
            LibPath = path;
        }

        public XApiWrapper():base()
        {
            base.OnConnectionStatus = OnConnectionStatus_callback;
            base.OnRtnError = OnRtnError_callback;

            base.OnRtnDepthMarketData = OnRtnDepthMarketData_callback;
            base.OnRtnQuoteRequest = OnRtnQuoteRequest_callback;

            base.OnRspQryInstrument = OnRspQryInstrument_callback;
            base.OnRspQryTradingAccount = OnRspQryTradingAccount_callback;
            base.OnRspQryInvestorPosition = OnRspQryInvestorPosition_callback;
            base.OnRspQrySettlementInfo = OnRspQrySettlementInfo_callback;
            base.OnRtnOrder = OnRtnOrder_callback;
            base.OnRtnTrade = OnRtnTrade_callback;
            base.OnRtnQuote = OnRtnQuote_callback;

            base.OnRspQryHistoricalTicks = OnRspQryHistoricalTicks_callback;
            base.OnRspQryHistoricalBars = OnRspQryHistoricalBars_callback;
        }

        public void Show()
        {
            MessageBox.Show("123456789");
        }

        private void OnConnectionStatus_callback(object sender, ConnectionStatus status, ref RspUserLoginField userLogin, int size1)
        {
            if (null != OnConnectionStatus)
            {
                OnConnectionStatus(this, new OnConnectionStatusEventArgs(status, ref userLogin, size1));
            }
        }

        private void OnRtnError_callback(object sender, ref ErrorField error)
        {
            if (null != OnRtnError)
            {
                OnRtnError(this, new OnRtnErrorEventArgs(ref error));
            }
        }

        private void OnRtnDepthMarketData_callback(object sender, ref DepthMarketDataNClass marketData)
        {
            if (null != OnRtnDepthMarketData)
            {
                OnRtnDepthMarketData(this, new OnRtnDepthMarketDataNEventArgs(ref marketData));
            }
        }

        private void OnRtnQuoteRequest_callback(object sender, ref QuoteRequestField quoteRequest)
        {
            if (null != OnRtnQuoteRequest)
            {
                OnRtnQuoteRequest(this, new OnRtnQuoteRequestEventArgs(ref quoteRequest));
            }
        }

        private void OnRspQryInstrument_callback(object sender, ref InstrumentField instrument, int size1, bool bIsLast)
        {
            if (null != OnRspQryInstrument)
            {
                OnRspQryInstrument(this, new OnRspQryInstrumentEventArgs(ref instrument, size1, bIsLast));
            }
        }

        private void OnRspQryTradingAccount_callback(object sender, ref AccountField account, int size1, bool bIsLast)
        {
            if (null != OnRspQryTradingAccount)
            {
                OnRspQryTradingAccount(this, new OnRspQryTradingAccountEventArgs(ref account, size1, bIsLast));
            }
        }

        private void OnRspQryInvestorPosition_callback(object sender, ref PositionField position, int size1, bool bIsLast)
        {
            if (null != OnRspQryInvestorPosition)
            {
                OnRspQryInvestorPosition(this, new OnRspQryInvestorPositionEventArgs(ref position, size1, bIsLast));
            }
        }
        private void OnRspQrySettlementInfo_callback(object sender, ref SettlementInfoField settlementInfo, int size1, bool bIsLast)
        {
            if (null != OnRspQrySettlementInfo)
            {
                OnRspQrySettlementInfo(this, new OnRspQrySettlementInfoEventArgs(ref settlementInfo, size1, bIsLast));
            }
        }
        private void OnRtnOrder_callback(object sender, ref OrderField order)
        {
            if (null != OnRtnOrder)
            {
                OnRtnOrder(this, new OnRtnOrderEventArgs(ref order));
            }
        }

        private void OnRtnTrade_callback(object sender, ref TradeField trade)
        {
            if (null != OnRtnTrade)
            {
                OnRtnTrade(this, new OnRtnTradeEventArgs(ref trade));
            }
        }

        private void OnRtnQuote_callback(object sender, ref QuoteField quote)
        {
            if (null != OnRtnQuote)
            {
                OnRtnQuote(this, new OnRtnQuoteEventArgs(ref quote));
            }
        }

        private void OnRspQryHistoricalTicks_callback(object sender, IntPtr pTicks, int size1, ref HistoricalDataRequestField request, int size2, bool bIsLast)
        {
            if (null != OnRspQryHistoricalTicks)
            {
                OnRspQryHistoricalTicks(this, new OnRspQryHistoricalTicksEventArgs(pTicks, size1, ref request, size2, bIsLast));
            }
        }

        private void OnRspQryHistoricalBars_callback(object sender, IntPtr pTicks, int size1, ref HistoricalDataRequestField request, int size2, bool bIsLast)
        {
            if (null != OnRspQryHistoricalBars)
            {
                OnRspQryHistoricalBars(this, new OnRspQryHistoricalBarsEventArgs(pTicks, size1, ref request, size2, bIsLast));
            }
        }

        private void OnRspQryInvestor_callback(object sender, ref InvestorField investor, int size1, bool bIsLast)
        {
            if (null != OnRspQryInvestor)
            {
                OnRspQryInvestor(this, new OnRspQryInvestorEventArgs(ref investor, size1, bIsLast));
            }
        }
    }
}
