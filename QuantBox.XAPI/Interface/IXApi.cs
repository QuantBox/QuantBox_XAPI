using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace QuantBox.XAPI.Interface
{
    public interface IXApi
    {
        //DelegateOnConnectionStatus OnConnectionStatus;
        //DelegateOnRtnError OnRtnError;

        void Connect();
    }
}
