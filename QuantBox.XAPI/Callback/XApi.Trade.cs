using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;

namespace QuantBox.XAPI.Callback
{
    public partial class XApi
    {
        public DelegateOnRspQryTradingAccount OnRspQryTradingAccount;
        public DelegateOnRspQryInvestorPosition OnRspQryInvestorPosition;
        public DelegateOnRspQrySettlementInfo OnRspQrySettlementInfo;
        public DelegateOnRtnOrder OnRtnOrder;
        public DelegateOnRtnTrade OnRtnTrade;
        public DelegateOnRtnQuote OnRtnQuote;

        private Dictionary<string, StringBuilder> dict = new Dictionary<string, StringBuilder>();

        public void ReqQryTradingAccount()
        {
            proxy.XRequest((byte)RequestType.ReqQryTradingAccount, Handle, IntPtr.Zero, 0, 0,
                IntPtr.Zero, 0, IntPtr.Zero, 0, IntPtr.Zero, 0);
        }

        public void ReqQryInvestorPosition(string szInstrument, string szExchange)
        {
            IntPtr szInstrumentPtr = Marshal.StringToHGlobalAnsi(szInstrument);
            IntPtr szExchangePtr = Marshal.StringToHGlobalAnsi(szExchange);

            proxy.XRequest((byte)RequestType.ReqQryInvestorPosition, Handle, IntPtr.Zero, 0, 0,
                szInstrumentPtr, 0, szExchangePtr, 0, IntPtr.Zero, 0);

            Marshal.FreeHGlobal(szInstrumentPtr);
            Marshal.FreeHGlobal(szExchangePtr);
        }

        public void ReqQrySettlementInfo(string szTradingDay)
        {
            dict.Remove(szTradingDay);

            IntPtr szTradingDayPtr = Marshal.StringToHGlobalAnsi(szTradingDay);

            proxy.XRequest((byte)RequestType.ReqQrySettlementInfo, Handle, IntPtr.Zero, 0, 0,
                szTradingDayPtr, 0, IntPtr.Zero, 0, IntPtr.Zero, 0);

            Marshal.FreeHGlobal(szTradingDayPtr);
        }

        public string SendOrder(int OrderRef, ref OrderField order1)
        {
            int size = Marshal.SizeOf(typeof(OrderField));

            IntPtr order1Ptr = Marshal.AllocHGlobal(size);
            //IntPtr order2Ptr = Marshal.AllocHGlobal(size);
            Marshal.StructureToPtr(order1, order1Ptr, false);
            //Marshal.StructureToPtr(order2, order2Ptr, false);

            IntPtr ptr = proxy.XRequest((byte)RequestType.ReqOrderInsert, Handle, IntPtr.Zero,
                OrderRef, 0,
                order1Ptr, size, IntPtr.Zero, 0, IntPtr.Zero, 0);

            Marshal.FreeHGlobal(order1Ptr);
            //Marshal.FreeHGlobal(order2Ptr);

            if (ptr.ToInt64() == 0)
                return null;

            return Marshal.PtrToStringAnsi(ptr);
        }

        public string SendOrder(int OrderRef, ref OrderField order1, ref OrderField order2)
        {
            int size = Marshal.SizeOf(typeof(OrderField));

            IntPtr order1Ptr = Marshal.AllocHGlobal(size);
            IntPtr order2Ptr = Marshal.AllocHGlobal(size);
            Marshal.StructureToPtr(order1, order1Ptr, false);
            Marshal.StructureToPtr(order2, order2Ptr, false);

            IntPtr ptr = proxy.XRequest((byte)RequestType.ReqOrderInsert, Handle, IntPtr.Zero,
                OrderRef, 0,
                order1Ptr, size, order2Ptr, size, IntPtr.Zero, 0);

            Marshal.FreeHGlobal(order1Ptr);
            Marshal.FreeHGlobal(order2Ptr);

            if (ptr.ToInt64() == 0)
                return null;

            return Marshal.PtrToStringAnsi(ptr);
        }

        public int CancelOrder(string szId)
        {
            IntPtr szIdPtr = Marshal.StringToHGlobalAnsi(szId);

            IntPtr ptr = proxy.XRequest((byte)RequestType.ReqOrderAction, Handle, IntPtr.Zero, 0, 0,
                szIdPtr, 0, IntPtr.Zero, 0, IntPtr.Zero, 0);

            Marshal.FreeHGlobal(szIdPtr);

            return ptr.ToInt32();
        }

        public string SendQuote(int QuoteRef, ref QuoteField quote)
        {
            int size = Marshal.SizeOf(typeof(QuoteField));

            IntPtr quotePtr = Marshal.AllocHGlobal(size);
            Marshal.StructureToPtr(quote, quotePtr, false);

            IntPtr ptr = proxy.XRequest((byte)RequestType.ReqQuoteInsert, Handle, IntPtr.Zero,
                QuoteRef, 0,
                quotePtr, size, IntPtr.Zero, 0, IntPtr.Zero, 0);

            Marshal.FreeHGlobal(quotePtr);

            if (ptr.ToInt64() == 0)
                return null;

            return Marshal.PtrToStringAnsi(ptr);
        }

        public int CancelQuote(string szId)
        {
            IntPtr szIdPtr = Marshal.StringToHGlobalAnsi(szId);

            IntPtr ptr = proxy.XRequest((byte)RequestType.ReqQuoteAction, Handle, IntPtr.Zero, 0, 0,
                szIdPtr, 0, IntPtr.Zero, 0, IntPtr.Zero, 0);

            Marshal.FreeHGlobal(szIdPtr);

            return ptr.ToInt32();
        }

        private void _OnRspQryTradingAccount(IntPtr ptr1, int size1, double double1)
        {
            if (OnRspQryTradingAccount == null)
                return;

            AccountField obj = PInvokeUtility.GetObjectFromIntPtr<AccountField>(ptr1);

            OnRspQryTradingAccount(this, ref obj, size1, double1 != 0);
        }

        private void _OnRspQryInvestorPosition(IntPtr ptr1, int size1, double double1)
        {
            if (OnRspQryInvestorPosition == null)
                return;

            PositionField obj = PInvokeUtility.GetObjectFromIntPtr<PositionField>(ptr1);

            OnRspQryInvestorPosition(this, ref obj, size1, double1 != 0);
        }

        private void _OnRspQrySettlementInfo(IntPtr ptr1, int size1, double double1)
        {
            if (OnRspQrySettlementInfo == null)
                return;

            SettlementInfoField obj = PInvokeUtility.GetObjectFromIntPtr<SettlementInfoField>(ptr1);

            OnRspQrySettlementInfo(this, ref obj, size1, double1 != 0);
        }

        private void _OnRtnOrder(IntPtr ptr1, int size1)
        {
            if (OnRtnOrder == null)
                return;

            OrderField obj = PInvokeUtility.GetObjectFromIntPtr<OrderField>(ptr1);

            OnRtnOrder(this, ref obj);
        }

        private void _OnRtnTrade(IntPtr ptr1, int size1)
        {
            if (OnRtnTrade == null)
                return;

            TradeField obj = PInvokeUtility.GetObjectFromIntPtr<TradeField>(ptr1);

            OnRtnTrade(this, ref obj);
        }

        private void _OnRtnQuote(IntPtr ptr1, int size1)
        {
            if (OnRtnQuote == null)
                return;

            QuoteField obj = PInvokeUtility.GetObjectFromIntPtr<QuoteField>(ptr1);

            OnRtnQuote(this, ref obj);
        }
    }
}
