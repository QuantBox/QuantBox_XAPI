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

        public void ReqQryHistoricalTicks(ref HistoricalDataRequestField request)
        {
            IntPtr ptr = Marshal.AllocHGlobal(Marshal.SizeOf(typeof(HistoricalDataRequestField)));
            Marshal.StructureToPtr(request, ptr, false);

            proxy.XRequest((byte)RequestType.ReqQryHistoricalTicks, Handle, IntPtr.Zero, 0, 0,
                ptr, 0, IntPtr.Zero, 0, IntPtr.Zero, 0);

            Marshal.FreeHGlobal(ptr);
        }

        public void ReqQryHistoricalBars(ref HistoricalDataRequestField request)
        {
            IntPtr ptr = Marshal.AllocHGlobal(Marshal.SizeOf(typeof(HistoricalDataRequestField)));
            Marshal.StructureToPtr(request, ptr, false);

            proxy.XRequest((byte)RequestType.ReqQryHistoricalBars, Handle, IntPtr.Zero, 0, 0,
                ptr, 0, IntPtr.Zero, 0, IntPtr.Zero, 0);

            Marshal.FreeHGlobal(ptr);
        }


        private void _OnRspQryHistoricalTicks(IntPtr ptr1, int size1, double double1, double double2)
        {
            if (OnRspQryHistoricalTicks == null)
                return;

            DepthMarketDataField obj = PInvokeUtility.GetObjectFromIntPtr<DepthMarketDataField>(ptr1);

            OnRspQryHistoricalTicks(this, ref obj, size1, double1 != 0,(int)double2);
        }
        private void _OnRspQryHistoricalBars(IntPtr ptr1, int size1, double double1, double double2)
        {
            if (OnRspQryHistoricalBars == null)
                return;

            BarField obj = PInvokeUtility.GetObjectFromIntPtr<BarField>(ptr1);

            OnRspQryHistoricalBars(this, ref obj, size1, double1 != 0, (int)double2);
        }
    }
}
