using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace QuantBox.XAPI
{
    public delegate IntPtr XCall(byte type, IntPtr pApi1, IntPtr pApi2, double double1, double double2, IntPtr ptr1, int size1, IntPtr ptr2, int size2, IntPtr ptr3, int size3);

    public class Proxy
    {
        private InvokeBase _Invoke;
        private XCall _XRequest;
        private PlatformID _PlatformID;

        public Proxy(string path)
        {
            _PlatformID = Environment.OSVersion.Platform;

            if (_PlatformID == PlatformID.Unix)
            {
                _Invoke = new SoInvoke(path);
            }
            else
            {
                _Invoke = new DllInvoke(path);
            }
        }

        ~Proxy()
        {
            Dispose(false);
        }

        private bool disposed;
        //Implement IDisposable.
        public void Dispose()
        {
            Dispose(true);
            GC.SuppressFinalize(this);
            //base.Dispose();
        }

        protected virtual void Dispose(bool disposing)
        {
            if (!disposed)
            {
                if (disposing)
                {
                    // Free other state (managed objects).
                }
                // Free your own state (unmanaged objects).
                // Set large fields to null.
                if(_Invoke != null)
                    _Invoke.Dispose();
                _Invoke = null;
                _XRequest = null;
                disposed = true;
            }
            //base.Dispose(disposing);
        }

        public IntPtr XRequest(byte type, IntPtr pApi1, IntPtr pApi2, double double1, double double2, IntPtr ptr1, int size1, IntPtr ptr2, int size2, IntPtr ptr3, int size3)
        {
            if (_XRequest == null)
            {
                if (_Invoke == null)
                    return IntPtr.Zero;

				_XRequest = (XCall)_Invoke.Invoke("XRequest", typeof(XCall));
                if (_XRequest == null)
                    return IntPtr.Zero;
            }
            return _XRequest(type, pApi1, pApi2, double1, double2, ptr1, size1, ptr2, size2, ptr3, size3);
        }
    }
}
