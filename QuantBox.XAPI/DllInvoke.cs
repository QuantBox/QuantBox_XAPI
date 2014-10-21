using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;

namespace QuantBox.XAPI
{
    public class DllInvoke:InvokeBase
    {
        [System.Flags]
        public enum LoadLibraryFlags : uint
        {
            DONT_RESOLVE_DLL_REFERENCES = 0x00000001,
            LOAD_IGNORE_CODE_AUTHZ_LEVEL = 0x00000010,
            LOAD_LIBRARY_AS_DATAFILE = 0x00000002,
            LOAD_LIBRARY_AS_DATAFILE_EXCLUSIVE = 0x00000040,
            LOAD_LIBRARY_AS_IMAGE_RESOURCE = 0x00000020,
            LOAD_WITH_ALTERED_SEARCH_PATH = 0x00000008
        }

        [DllImport("kernel32.dll")]
        private extern static IntPtr LoadLibrary(String lpFileName);
        [DllImport("kernel32.dll")]
        private extern static IntPtr GetProcAddress(IntPtr hModule, String lpProcName);
        [DllImport("kernel32.dll")]
        private extern static bool FreeLibrary(IntPtr hModule);
        [DllImport("kernel32.dll", SetLastError = true)]
        public static extern int GetDllDirectory(int bufSize, StringBuilder buf);
        [DllImport("kernel32.dll", SetLastError = true)]
        public static extern bool SetDllDirectory(string lpPathName);
        [DllImport("kernel32.dll", SetLastError = true)]
        public static extern IntPtr LoadLibraryEx(string lpFileName, IntPtr hReservedNull, LoadLibraryFlags dwFlags);

        public DllInvoke(String DLLPath)
        {
            hLib = LoadLibraryEx(DLLPath,IntPtr.Zero,LoadLibraryFlags.LOAD_WITH_ALTERED_SEARCH_PATH);
            if (hLib == IntPtr.Zero)
            {
                throw new Exception(string.Format("无法加载 {0}", DLLPath));
            }
        }
        ~DllInvoke()
        {
            Dispose();
        }
        //将要执行的函数转换为委托
        public override Delegate Invoke(String APIName, Type t)
        {
            if (hLib == IntPtr.Zero)
                return (Delegate)null;

            IntPtr api = GetProcAddress(hLib, APIName);
            return (Delegate)Marshal.GetDelegateForFunctionPointer(api, t);
        }

        public override void Dispose()
        {
            if (hLib != IntPtr.Zero)
                FreeLibrary(hLib);
            hLib = IntPtr.Zero;
        }
    }
}
