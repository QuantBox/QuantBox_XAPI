using QuantBox.XAPI.Interface;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;

namespace QuantBox.XAPI.Callback
{
    public partial class XApi : IXMarketData
    {
        public DelegateOnRtnDepthMarketData OnRtnDepthMarketData;

        #region 已经订阅的行情
        private Dictionary<string, SortedSet<string>> _SubscribedInstruments = new Dictionary<string, SortedSet<string>>();
        public Dictionary<string, SortedSet<string>> SubscribedInstruments
        {
            get
            {
                lock (locker)
                {
                    return _SubscribedInstruments;
                }
            }
        }
        #endregion

        #region 订阅行情
        public virtual void Subscribe(string szInstrument, string szExchange)
        {
            lock (locker)
            {
                IntPtr szInstrumentPtr = Marshal.StringToHGlobalAnsi(szInstrument);
                IntPtr szExchangePtr = Marshal.StringToHGlobalAnsi(szExchange);

                proxy.XRequest((byte)RequestType.Subscribe, Handle, IntPtr.Zero, 0, 0,
                    szInstrumentPtr, 0, szExchangePtr, 0, IntPtr.Zero, 0);

                SortedSet<string> instruments;
                if(!_SubscribedInstruments.TryGetValue(szExchange, out instruments))
                {
                    instruments = new SortedSet<string>();
                    _SubscribedInstruments[szExchange] = instruments;
                }                

                szInstrument.Split(new char[2] { ';', ',' }).ToList().ForEach(x =>
                {
                    instruments.Add(x);
                });

                Marshal.FreeHGlobal(szInstrumentPtr);
                Marshal.FreeHGlobal(szExchangePtr);
            }
        }

        public virtual void Unsubscribe(string szInstrument, string szExchange)
        {
            lock (locker)
            {
                IntPtr szInstrumentPtr = Marshal.StringToHGlobalAnsi(szInstrument);
                IntPtr szExchangePtr = Marshal.StringToHGlobalAnsi(szExchange);

                proxy.XRequest((byte)RequestType.Unsubscribe, Handle, IntPtr.Zero, 0, 0,
                    szInstrumentPtr, 0, szExchangePtr, 0, IntPtr.Zero, 0);

                SortedSet<string> instruments;
                if (!_SubscribedInstruments.TryGetValue(szExchange, out instruments))
                {
                    instruments = new SortedSet<string>();
                    _SubscribedInstruments[szExchange] = instruments;
                }

                szInstrument.Split(new char[2] { ';', ',' }).ToList().ForEach(x =>
                {
                    instruments.Remove(x);
                });

                Marshal.FreeHGlobal(szInstrumentPtr);
                Marshal.FreeHGlobal(szExchangePtr);
            }
        }
        #endregion

        private void _OnRtnDepthMarketData(IntPtr ptr1)
        {
            DepthMarketDataField obj = (DepthMarketDataField)Marshal.PtrToStructure(ptr1, typeof(DepthMarketDataField));

            OnRtnDepthMarketData(this, ref obj);
        }
    }
}
