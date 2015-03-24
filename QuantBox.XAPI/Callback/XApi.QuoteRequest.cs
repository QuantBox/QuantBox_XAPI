using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;

namespace QuantBox.XAPI.Callback
{
    public partial class XApi
    {
        public DelegateOnRtnQuoteRequest OnRtnQuoteRequest
        {
            get {return OnRtnQuoteRequest_;}
            set { OnRtnQuoteRequest_ = value; }
        }
        private DelegateOnRtnQuoteRequest OnRtnQuoteRequest_;

        #region 做市商询价订阅，主要是XSpeed这个功能是在交易API中
        public int MaxSubscribedQuotesCount;
        public int SubscribedQuotesCount
        {
            get
            {
                lock (locker)
                {
                    int cnt = 0;
                    foreach (var kv in _SubscribedQuotes)
                    {
                        cnt += kv.Value.Count;
                    }
                    return cnt;
                }
            }
        }

        public bool SubscribedQuotesContains(string szInstrument, string szExchange)
        {
            lock (locker)
            {
                SortedSet<string> instruments;
                if (_SubscribedQuotes.TryGetValue(szExchange, out instruments))
                {
                    return instruments.Contains(szInstrument);
                }
                return false;
            }
        }

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


        private void _OnRtnQuoteRequest(IntPtr ptr1, int size1)
        {
            if (OnRtnQuoteRequest_ == null)
                return;

            //QuoteRequestField obj = PInvokeUtility.GetObjectFromIntPtr<QuoteRequestField>(ptr1);
            QuoteRequestField obj = (QuoteRequestField)Marshal.PtrToStructure(ptr1, typeof(QuoteRequestField));

            OnRtnQuoteRequest_(this, ref obj);
        }
    }
}
