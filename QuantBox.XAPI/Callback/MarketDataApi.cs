using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;

namespace QuantBox.XAPI.Callback
{
    public class MarketDataApi : BaseApi,IDisposable
    {
        public DelegateOnRtnDepthMarketData OnRtnDepthMarketData;
        internal MarketDataApi(string path1, Queue queue)
            : base(path1, queue)
        {
        }

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

        protected override IntPtr OnRespone(byte type, IntPtr pApi1, IntPtr pApi2, double double1, double double2, IntPtr ptr1, int size1, IntPtr ptr2, int size2, IntPtr ptr3, int size3)
        {
            switch ((ResponeType)type)
            {
                case ResponeType.OnRtnDepthMarketData:
                    _OnRtnDepthMarketData(ptr1);
                    break;
                default:
                    break;
            }

            return IntPtr.Zero;
        }

        private void _OnRtnDepthMarketData(IntPtr ptr1)
        {
            DepthMarketDataField obj = (DepthMarketDataField)Marshal.PtrToStructure(ptr1, typeof(DepthMarketDataField));

            OnRtnDepthMarketData(this, ref obj);
        }

        public override void Connect()
        {
            base.Connect();

            foreach (var kv in SubscribedInstruments)
            {
                foreach(var kvv in kv.Value)
                {
                    Subscribe(kvv, kv.Key);
                }
            }
        }
    }
}
