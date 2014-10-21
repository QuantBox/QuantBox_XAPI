using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;

namespace QuantBox.XAPI
{
    public class SoInvoke : InvokeBase
    {
        [DllImport("libdl.so")]
        private extern static IntPtr dlopen(String path, int mode);
        [DllImport("libdl.so")]
        private extern static IntPtr dlsym(IntPtr handle, String symbol);
        [DllImport("libdl.so")]
        private extern static IntPtr dlerror();
        [DllImport("libdl.so")]
        private extern static int dlclose(IntPtr handle);

        public SoInvoke(String DLLPath)
        {
            hLib = dlopen(DLLPath, 1);
            if (hLib == IntPtr.Zero)
            {
                IntPtr errptr = dlerror();
                string error = Marshal.PtrToStringAnsi(errptr);
                throw new Exception(error);
            }
        }
        ~SoInvoke()
        {
            Dispose();
        }
        //将要执行的函数转换为委托
        public override Delegate Invoke(String APIName, Type t)
        {
            if (hLib == IntPtr.Zero)
                return (Delegate)null;

            IntPtr api = dlsym(hLib, APIName);
            return (Delegate)Marshal.GetDelegateForFunctionPointer(api, t);
        }

        public override void Dispose()
        {
            if (hLib != IntPtr.Zero)
                dlclose(hLib);
            hLib = IntPtr.Zero;
        }
    }
}
