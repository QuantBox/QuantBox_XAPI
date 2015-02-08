using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;

namespace QuantBox.XAPI
{
    public class PInvokeUtility
    {
        [DllImport("kernel32.dll", CharSet = CharSet.Auto)]
        public static extern void OutputDebugString(string message);

        static Encoding encodingGB2312 = Encoding.GetEncoding(936);

        public static string GetUnicodeString(byte[] str)
        {
            if(str == null)
            {
                return string.Empty;
            }
            int bytecount = 0;
            foreach(byte b in str)
            {
                if (0 == b)
                    break;
                ++bytecount;
            }
            if (0 == bytecount)
                return string.Empty;
            //比TrimEnd('\0');快,减少了内存的复制
            return encodingGB2312.GetString(str, 0, bytecount);
        }

        public static T GetObjectFromIntPtr<T>(IntPtr handler)
        {
            if (handler == IntPtr.Zero)
            {
              return default(T);
            }
            else
            {
              return (T)Marshal.PtrToStructure(handler, typeof(T));
            }
        }
    }
}
