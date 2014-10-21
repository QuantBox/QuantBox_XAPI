using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace QuantBox.XAPI
{
    public class InvokeBase
    {
        protected IntPtr hLib = IntPtr.Zero;
        public virtual Delegate Invoke(String APIName, Type t)
        {
            return null;
        }

        public virtual void Dispose()
        {

        }
    }
}
