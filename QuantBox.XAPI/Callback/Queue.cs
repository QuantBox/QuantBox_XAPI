using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;

namespace QuantBox.XAPI.Callback
{
    public class Queue :IDisposable
    {
        private Proxy proxy;
        public IntPtr Handle = IntPtr.Zero;
        protected object locker = new object();
        public Queue(string path)
        {
            proxy = new Proxy(path);
            Handle = new IntPtr((long)proxy.XRequest((byte)RequestType.Create, IntPtr.Zero, IntPtr.Zero, 0, 0, IntPtr.Zero, 0, IntPtr.Zero, 0, IntPtr.Zero, 0));
        }

        ~Queue()
        {
            Dispose(false);
        }

        private bool disposed;

        public void Dispose()
        {
            Dispose(true);
            GC.SuppressFinalize(this);
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
                Disconnect();
                disposed = true;
            }
            //base.Dispose(disposing);
        }

        public void Connect()
        {
            lock (locker)
            {
                if (proxy != null)
                {
                    // 启动消息队列循环
                    proxy.XRequest((byte)RequestType.Connect, Handle, IntPtr.Zero, 0, 0, IntPtr.Zero, 0, IntPtr.Zero, 0, IntPtr.Zero, 0);
                }
            }
        }

        public void Disconnect()
        {
            lock(locker)
            {
                if (proxy != null)
                {
                    Register(IntPtr.Zero);

                    //停止底层线程
                    proxy.XRequest((byte)RequestType.Disconnect, Handle, IntPtr.Zero, 0, 0, IntPtr.Zero, 0, IntPtr.Zero, 0, IntPtr.Zero, 0);

                    proxy.XRequest((byte)RequestType.Release, Handle, IntPtr.Zero, 0, 0, IntPtr.Zero, 0, IntPtr.Zero, 0, IntPtr.Zero, 0);

                    proxy.Dispose();
                }
                proxy = null;
                Handle = IntPtr.Zero;
            }
        }

        public void Register(IntPtr ptr1)
        {
            lock(this)
            {
                if (proxy != null)
                {
                    proxy.XRequest((byte)RequestType.Register, Handle, IntPtr.Zero, 0, 0, ptr1, 0, IntPtr.Zero, 0, IntPtr.Zero, 0);
                }
            }
        }
    }
}
