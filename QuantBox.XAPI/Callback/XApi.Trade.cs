using QuantBox.XAPI.Interface;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;

namespace QuantBox.XAPI.Callback
{
    public partial class XApi : IXTrade
    {
        public DelegateOnRspQryTradingAccount OnRspQryTradingAccount
        {
            get { return OnRspQryTradingAccount_; }
            set { OnRspQryTradingAccount_ = value; }
        }
        public DelegateOnRspQryInvestorPosition OnRspQryInvestorPosition
        {
            get { return OnRspQryInvestorPosition_; }
            set { OnRspQryInvestorPosition_ = value; }
        }
        public DelegateOnRspQrySettlementInfo OnRspQrySettlementInfo
        {
            get { return OnRspQrySettlementInfo_; }
            set { OnRspQrySettlementInfo_ = value; }
        }
        public DelegateOnRspQryInvestor OnRspQryInvestor
        {
            get { return OnRspQryInvestor_; }
            set { OnRspQryInvestor_ = value; }
        }
        public DelegateOnRtnOrder OnRtnOrder
        {
            get { return OnRtnOrder_; }
            set { OnRtnOrder_ = value; }
        }
        public DelegateOnRtnTrade OnRtnTrade
        {
            get { return OnRtnTrade_; }
            set { OnRtnTrade_ = value; }
        }
        public DelegateOnRtnQuote OnRtnQuote
        {
            get { return OnRtnQuote_; }
            set { OnRtnQuote_ = value; }
        }

        private DelegateOnRtnOrder OnRtnOrder_;
        private DelegateOnRtnTrade OnRtnTrade_;
        private DelegateOnRtnQuote OnRtnQuote_;
        private DelegateOnRspQryTradingAccount OnRspQryTradingAccount_;
        private DelegateOnRspQryInvestorPosition OnRspQryInvestorPosition_;
        private DelegateOnRspQrySettlementInfo OnRspQrySettlementInfo_;
        private DelegateOnRspQryInvestor OnRspQryInvestor_;

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

        public void SendOrder(int OrderRef, ref OrderField[] orders,out string[] OrderRefs)
        {
            int OrderField_size = Marshal.SizeOf(typeof(OrderField));
            int OrderIDType_size = Marshal.SizeOf(typeof(OrderIDType));
            
            IntPtr OrderField_Ptr = Marshal.AllocHGlobal(OrderField_size * orders.Length);
            IntPtr OrderIDType_Ptr = Marshal.AllocHGlobal(OrderIDType_size * orders.Length);

            // 将结构体写成内存块
            for (int i = 0; i < orders.Length;++i)
            {
                Marshal.StructureToPtr(orders[i], OrderField_Ptr + i * OrderField_size, false);
            }

            IntPtr ptr = proxy.XRequest((byte)RequestType.ReqOrderInsert, Handle, OrderIDType_Ptr,
                OrderRef, 0,
                OrderField_Ptr, orders.Length, IntPtr.Zero, 0, IntPtr.Zero, 0);

            OrderRefs = new string[orders.Length];

            for(int i = 0;i<orders.Length;++i)
            {
                // 这里定义一个ID占64字节
                OrderIDType output = (OrderIDType)Marshal.PtrToStructure(OrderIDType_Ptr + i * OrderIDType_size, typeof(OrderIDType));

                OrderRefs[i] = output.ID;
            }

            Marshal.FreeHGlobal(OrderField_Ptr);
            Marshal.FreeHGlobal(OrderIDType_Ptr);
        }

        public void CancelOrder(string[] szId,out string[] errs)
        {
            int OrderIDType_size = Marshal.SizeOf(typeof(OrderIDType));

            IntPtr Input_Ptr = Marshal.AllocHGlobal(OrderIDType_size * szId.Length);
            IntPtr Output_Ptr = Marshal.AllocHGlobal(OrderIDType_size * szId.Length);

            // 将结构体写成内存块
            for (int i = 0; i < szId.Length; ++i)
            {
                OrderIDType _szId = new OrderIDType();
                _szId.ID = szId[i];
                Marshal.StructureToPtr(_szId, Input_Ptr + i * OrderIDType_size, false);
            }

            IntPtr ptr = proxy.XRequest((byte)RequestType.ReqOrderAction, Handle, Output_Ptr, 0, 0,
                Input_Ptr, szId.Length, IntPtr.Zero, 0, IntPtr.Zero, 0);

            errs = new string[szId.Length];

            for (int i = 0; i < szId.Length; ++i)
            {
                // 这里定义一个ID占64字节
                OrderIDType output = (OrderIDType)Marshal.PtrToStructure(Output_Ptr + i * OrderIDType_size, typeof(OrderIDType));

                errs[i] = output.ID;
            }

            Marshal.FreeHGlobal(Input_Ptr);
            Marshal.FreeHGlobal(Output_Ptr);
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
            if (OnRspQryTradingAccount_ == null)
                return;

            AccountField obj = PInvokeUtility.GetObjectFromIntPtr<AccountField>(ptr1);

            OnRspQryTradingAccount_(this, ref obj, size1, double1 != 0);
        }

        private void _OnRspQryInvestorPosition(IntPtr ptr1, int size1, double double1)
        {
            if (OnRspQryInvestorPosition_ == null)
                return;

            PositionField obj = PInvokeUtility.GetObjectFromIntPtr<PositionField>(ptr1);

            OnRspQryInvestorPosition_(this, ref obj, size1, double1 != 0);
        }

        private void _OnRspQrySettlementInfo(IntPtr ptr1, int size1, double double1)
        {
            if (OnRspQrySettlementInfo_ == null)
                return;

            SettlementInfoField obj = PInvokeUtility.GetObjectFromIntPtr<SettlementInfoField>(ptr1);

            OnRspQrySettlementInfo_(this, ref obj, size1, double1 != 0);
        }

        private void _OnRspQryInvestor(IntPtr ptr1, int size1, double double1)
        {
            if (OnRspQryInvestor_ == null)
                return;

            InvestorField obj = PInvokeUtility.GetObjectFromIntPtr<InvestorField>(ptr1);

            OnRspQryInvestor_(this, ref obj, size1, double1 != 0);
        }

        private void _OnRtnOrder(IntPtr ptr1, int size1)
        {
            // 求快，不加
            if (OnRtnOrder_ == null)
                return;

            OrderField obj = (OrderField)Marshal.PtrToStructure(ptr1, typeof(OrderField));

            OnRtnOrder_(this, ref obj);
        }

        private void _OnRtnTrade(IntPtr ptr1, int size1)
        {
            // 求快，不加
            if (OnRtnTrade_ == null)
                return;

            TradeField obj = (TradeField)Marshal.PtrToStructure(ptr1, typeof(TradeField));

            OnRtnTrade_(this, ref obj);
        }

        private void _OnRtnQuote(IntPtr ptr1, int size1)
        {
            if (OnRtnQuote_ == null)
                return;

            QuoteField obj = (QuoteField)Marshal.PtrToStructure(ptr1, typeof(QuoteField));

            OnRtnQuote_(this, ref obj);
        }
    }
}
