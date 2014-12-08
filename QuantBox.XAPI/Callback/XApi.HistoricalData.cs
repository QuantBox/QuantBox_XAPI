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

        public int ReqQryHistoricalTicks(ref HistoricalDataRequestField request)
        {
            IntPtr ptr = Marshal.AllocHGlobal(Marshal.SizeOf(typeof(HistoricalDataRequestField)));
            Marshal.StructureToPtr(request, ptr, false);

            IntPtr ret = proxy.XRequest((byte)RequestType.ReqQryHistoricalTicks, Handle, IntPtr.Zero, 0, 0,
                ptr, 0, IntPtr.Zero, 0, IntPtr.Zero, 0);

            Marshal.FreeHGlobal(ptr);

            return ret.ToInt32();
        }

        public int ReqQryHistoricalBars(ref HistoricalDataRequestField request)
        {
            IntPtr ptr = Marshal.AllocHGlobal(Marshal.SizeOf(typeof(HistoricalDataRequestField)));
            Marshal.StructureToPtr(request, ptr, false);

            IntPtr ret = proxy.XRequest((byte)RequestType.ReqQryHistoricalBars, Handle, IntPtr.Zero, 0, 0,
                ptr, 0, IntPtr.Zero, 0, IntPtr.Zero, 0);

            Marshal.FreeHGlobal(ptr);

            return ret.ToInt32();
        }


        private void _OnRspQryHistoricalTicks(IntPtr ptr1, int size1, IntPtr ptr2, int size2, double double1)
        {
            if (OnRspQryHistoricalTicks == null)
                return;

            HistoricalDataRequestField obj = PInvokeUtility.GetObjectFromIntPtr<HistoricalDataRequestField>(ptr2);

            OnRspQryHistoricalTicks(this, ptr1, size1,ref obj, size2, double1 != 0);
        }
        private void _OnRspQryHistoricalBars(IntPtr ptr1, int size1, IntPtr ptr2, int size2, double double1)
        {
            if (OnRspQryHistoricalBars == null)
                return;

            HistoricalDataRequestField obj = PInvokeUtility.GetObjectFromIntPtr<HistoricalDataRequestField>(ptr2);

            OnRspQryHistoricalBars(this, ptr1, size1,ref obj, size2, double1 != 0);
        }
    }
}
