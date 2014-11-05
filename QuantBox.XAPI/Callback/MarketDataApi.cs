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
        public DelegateOnRtnQuoteRequest OnRtnQuoteRequest;

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

        #region 做市商询价订阅，主要是XSpeed这个功能是在交易API中
        private Dictionary<string, SortedSet<string>> _SubscribedQuotes = new Dictionary<string, SortedSet<string>>();
        public Dictionary<string, SortedSet<string>> SubscribedQuotes
        {
            get
            {
                lock (locker)
                {
                    return _SubscribedQuotes;
                }
            }
        }

        public virtual void SubscribeQuote(string szInstrument, string szExchange)
        {
            lock (locker)
            {
                IntPtr szInstrumentPtr = Marshal.StringToHGlobalAnsi(szInstrument);
                IntPtr szExchangePtr = Marshal.StringToHGlobalAnsi(szExchange);

                proxy.XRequest((byte)RequestType.SubscribeQuote, Handle, IntPtr.Zero, 0, 0,
                    szInstrumentPtr, 0, szExchangePtr, 0, IntPtr.Zero, 0);

                SortedSet<string> instruments;
                if (!_SubscribedQuotes.TryGetValue(szExchange, out instruments))
                {
                    instruments = new SortedSet<string>();
                    _SubscribedQuotes[szExchange] = instruments;
                }

                szInstrument.Split(new char[2] { ';', ',' }).ToList().ForEach(x =>
                {
                    instruments.Add(x);
                });

                Marshal.FreeHGlobal(szInstrumentPtr);
                Marshal.FreeHGlobal(szExchangePtr);
            }
        }

        public virtual void UnsubscribeQuote(string szInstrument, string szExchange)
        {
            lock (locker)
            {
                IntPtr szInstrumentPtr = Marshal.StringToHGlobalAnsi(szInstrument);
                IntPtr szExchangePtr = Marshal.StringToHGlobalAnsi(szExchange);

                proxy.XRequest((byte)RequestType.UnsubscribeQuote, Handle, IntPtr.Zero, 0, 0,
                    szInstrumentPtr, 0, szExchangePtr, 0, IntPtr.Zero, 0);

                SortedSet<string> instruments;
                if (!_SubscribedQuotes.TryGetValue(szExchange, out instruments))
                {
                    instruments = new SortedSet<string>();
                    _SubscribedQuotes[szExchange] = instruments;
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
                case ResponeType.OnRtnQuoteRequest:
                    _OnRtnQuoteRequest(ptr1, size1);
                    break;
                default:
                    base.OnRespone(type, pApi1, pApi2, double1, double2, ptr1, size1, ptr2, size2, ptr3, size3);
                    break;
            }

            return IntPtr.Zero;
        }

        private void _OnRtnDepthMarketData(IntPtr ptr1)
        {
            DepthMarketDataField obj = (DepthMarketDataField)Marshal.PtrToStructure(ptr1, typeof(DepthMarketDataField));

            OnRtnDepthMarketData(this, ref obj);
        }

        private void _OnRtnQuoteRequest(IntPtr ptr1, int size1)
        {
            if (OnRtnQuoteRequest == null)
                return;

            QuoteRequestField obj = PInvokeUtility.GetObjectFromIntPtr<QuoteRequestField>(ptr1);

            OnRtnQuoteRequest(this, ref obj);
        }

        public override void Connect()
        {
            base.Connect();

            lock (locker)
            {

                foreach (var kv in _SubscribedInstruments)
                {
                    foreach (var kvv in kv.Value.ToList())
                    {
                        Subscribe(kvv, kv.Key);
                    }
                }

                // 做市商询价
                foreach (var kv in _SubscribedQuotes)
                {
                    foreach (var kvv in kv.Value.ToList())
                    {
                        SubscribeQuote(kvv, kv.Key);
                    }
                }
            }
        }
    }
}
