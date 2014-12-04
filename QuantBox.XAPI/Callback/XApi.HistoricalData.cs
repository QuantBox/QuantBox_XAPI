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
        public DelegateOnRspQryHistoricalTicks OnRspQryHistoricalTicks;
        public DelegateOnRspQryHistoricalBars OnRspQryHistoricalBars;

        public void ReqQryHistoricalTicks(string szInstrument, string szExchange,int datetime1,int datetime2)
        {
            IntPtr szInstrumentPtr = Marshal.StringToHGlobalAnsi(szInstrument);
            IntPtr szExchangePtr = Marshal.StringToHGlobalAnsi(szExchange);

            proxy.XRequest((byte)RequestType.ReqQryHistoricalTicks, Handle, IntPtr.Zero, 0, 0,
                szInstrumentPtr, datetime1, szExchangePtr, datetime2, IntPtr.Zero, 0);

            Marshal.FreeHGlobal(szInstrumentPtr);
            Marshal.FreeHGlobal(szExchangePtr);
        }

        public void ReqQryHistoricalBars(string szInstrument, string szExchange, int datetime1, int datetime2,long barSize)
        {
            IntPtr szInstrumentPtr = Marshal.StringToHGlobalAnsi(szInstrument);
            IntPtr szExchangePtr = Marshal.StringToHGlobalAnsi(szExchange);

            proxy.XRequest((byte)RequestType.ReqQryHistoricalBars, Handle, IntPtr.Zero, barSize, 0,
                szInstrumentPtr, datetime1, szExchangePtr, datetime2, IntPtr.Zero, 0);

            Marshal.FreeHGlobal(szInstrumentPtr);
            Marshal.FreeHGlobal(szExchangePtr);
        }


        private void _OnRspQryHistoricalTicks(IntPtr ptr1, int size1, double double1)
        {
            if (OnRspQryHistoricalTicks == null)
                return;

            DepthMarketDataField obj = PInvokeUtility.GetObjectFromIntPtr<DepthMarketDataField>(ptr1);

            OnRspQryHistoricalTicks(this, ref obj, size1, double1 != 0);
        }
        private void _OnRspQryHistoricalBars(IntPtr ptr1, int size1, double double1)
        {
            if (OnRspQryHistoricalBars == null)
                return;

            BarField obj = PInvokeUtility.GetObjectFromIntPtr<BarField>(ptr1);

            OnRspQryHistoricalBars(this, ref obj, size1, double1 != 0);
        }
    }
}
