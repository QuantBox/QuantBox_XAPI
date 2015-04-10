using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace QuantBox.XAPI.Interface
{
    public interface IXMarketData
    {
        void Subscribe(string szInstrument, string szExchange);
        void Unsubscribe(string szInstrument, string szExchange);
    }
}
