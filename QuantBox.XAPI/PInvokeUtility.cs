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
        static Encoding encodingGB2312 = Encoding.GetEncoding(936);

        public static string GetUnicodeString(byte[] str)
        {
            if(str == null)
            {
                return string.Empty;
            }
            return encodingGB2312.GetString(str).TrimEnd('\0');
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
