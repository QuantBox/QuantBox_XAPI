using QuantBox.XAPI.Interface;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;

namespace QuantBox.XAPI.Callback
{
    public partial class XApi:IXInstrument
    {
        public DelegateOnRspQryInstrument OnRspQryInstrument
        {
            get { return OnRspQryInstrument_; }
            set { OnRspQryInstrument_ = value; }
        }
        private DelegateOnRspQryInstrument OnRspQryInstrument_;
        
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
            if (OnRspQryInstrument_ == null)
                return;

            InstrumentField obj = PInvokeUtility.GetObjectFromIntPtr<InstrumentField>(ptr1);

            OnRspQryInstrument_(this, ref obj, size1, double1 != 0);
        }
    }
}
