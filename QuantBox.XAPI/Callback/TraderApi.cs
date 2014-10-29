using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;

namespace QuantBox.XAPI.Callback
{
    public class TraderApi : MarketDataApi
    {
        public DelegateOnRspQryInstrument OnRspQryInstrument;
        public DelegateOnRspQryTradingAccount OnRspQryTradingAccount;
        public DelegateOnRspQrySettlementInfo OnRspQrySettlementInfo;
        public DelegateOnRtnOrder OnRtnOrder;
        public DelegateOnRtnTrade OnRtnTrade;

        private Dictionary<string, StringBuilder> dict = new Dictionary<string, StringBuilder>();
        internal TraderApi(string path1, Queue queue)
            : base(path1, queue)
        {
        }

        public void ReqQryInstrument(string szInstrument, string szExchange)
        {
            IntPtr szInstrumentPtr = Marshal.StringToHGlobalAnsi(szInstrument);
            IntPtr szExchangePtr = Marshal.StringToHGlobalAnsi(szExchange);

            proxy.XRequest((byte)RequestType.ReqQryInstrument, Handle, IntPtr.Zero, 0, 0,
                szInstrumentPtr, 0, szExchangePtr, 0, IntPtr.Zero, 0);

            Marshal.FreeHGlobal(szInstrumentPtr);
            Marshal.FreeHGlobal(szExchangePtr);
        }

        public void ReqQryTradingAccount()
        {
            proxy.XRequest((byte)RequestType.ReqQryTradingAccount, Handle, IntPtr.Zero, 0, 0,
                IntPtr.Zero, 0, IntPtr.Zero, 0, IntPtr.Zero, 0);
        }

        public void ReqQrySettlementInfo(string szTradingDay)
        {
            dict.Remove(szTradingDay);

            IntPtr szTradingDayPtr = Marshal.StringToHGlobalAnsi(szTradingDay);

            proxy.XRequest((byte)RequestType.ReqQrySettlementInfo, Handle, IntPtr.Zero, 0, 0,
                szTradingDayPtr, 0, IntPtr.Zero, 0, IntPtr.Zero, 0);

            Marshal.FreeHGlobal(szTradingDayPtr);
        }

        public string SendOrder(int OrderRef, ref OrderField order1, ref ulong ret)
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

        public string SendOrder(int OrderRef, ref OrderField order1, ref OrderField order2, ref ulong ret)
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

        public string SendQuote(ref OrderField order1, ref OrderField order2)
        {
            int size = Marshal.SizeOf(typeof(OrderField));

            IntPtr order1Ptr = Marshal.AllocHGlobal(size);
            IntPtr order2Ptr = Marshal.AllocHGlobal(size);
            Marshal.StructureToPtr(order1, order1Ptr, false);
            Marshal.StructureToPtr(order2, order2Ptr, false);

            proxy.XRequest((byte)RequestType.ReqQuoteInsert, Handle, IntPtr.Zero, 0, 0,
                IntPtr.Zero, 0, IntPtr.Zero, 0, IntPtr.Zero, 0);

            Marshal.FreeHGlobal(order1Ptr);
            Marshal.FreeHGlobal(order2Ptr);

            return string.Empty;
        }

        public void CancelQuote(string szId)
        {
            IntPtr szIdPtr = Marshal.StringToHGlobalAnsi(szId);

            proxy.XRequest((byte)RequestType.ReqQuoteAction, Handle, IntPtr.Zero, 0, 0,
                szIdPtr, 0, IntPtr.Zero, 0, IntPtr.Zero, 0);

            Marshal.FreeHGlobal(szIdPtr);
        }

        protected override IntPtr OnRespone(byte type, IntPtr pApi1, IntPtr pApi2, double double1, double double2, IntPtr ptr1, int size1, IntPtr ptr2, int size2, IntPtr ptr3, int size3)
        {
            switch ((ResponeType)type)
            {
                case ResponeType.OnRspQryInstrument:
                    _OnRspQryInstrument(ptr1,size1, double1);
                    break;
                case ResponeType.OnRspQryTradingAccount:
                    _OnRspQryTradingAccount(ptr1, size1, double1);
                    break;
                case ResponeType.OnRspQrySettlementInfo:
                    _OnRspQrySettlementInfo(ptr1, size1, double1);
                    break;
                case ResponeType.OnRtnOrder:
                    _OnRtnOrder(ptr1, size1);
                    break;
                case ResponeType.OnRtnTrade:
                    _OnRtnTrade(ptr1, size1);
                    break;
                default:
                    break;
            }

            return IntPtr.Zero;
        }

        private void _OnRspQryInstrument(IntPtr ptr1,int size1, double double1)
        {
            if (OnRspQryInstrument == null)
                return;

            InstrumentField obj = PInvokeUtility.GetObjectFromIntPtr<InstrumentField>(ptr1);

            OnRspQryInstrument(this, ref obj, size1, double1 != 0);
        }

        private void _OnRspQryTradingAccount(IntPtr ptr1, int size1, double double1)
        {
            if (OnRspQryTradingAccount == null)
                return;

            AccountField obj = PInvokeUtility.GetObjectFromIntPtr<AccountField>(ptr1);

            OnRspQryTradingAccount(this, ref obj, size1, double1 != 0);
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
    }
}
