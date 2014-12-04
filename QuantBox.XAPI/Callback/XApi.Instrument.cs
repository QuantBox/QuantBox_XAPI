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
        public DelegateOnRspQryInstrument OnRspQryInstrument;
        
        public void ReqQryInstrument(string szInstrument, string szExchange)
        {
            IntPtr szInstrumentPtr = Marshal.StringToHGlobalAnsi(szInstrument);
            IntPtr szExchangePtr = Marshal.StringToHGlobalAnsi(szExchange);

            proxy.XRequest((byte)RequestType.ReqQryInstrument, Handle, IntPtr.Zero, 0, 0,
                szInstrumentPtr, 0, szExchangePtr, 0, IntPtr.Zero, 0);

            Marshal.FreeHGlobal(szInstrumentPtr);
            Marshal.FreeHGlobal(szExchangePtr);
        }

        private void _OnRspQryInstrument(IntPtr ptr1,int size1, double double1)
        {
            if (OnRspQryInstrument == null)
                return;

            InstrumentField obj = PInvokeUtility.GetObjectFromIntPtr<InstrumentField>(ptr1);

            OnRspQryInstrument(this, ref obj, size1, double1 != 0);
        }
    }
}
