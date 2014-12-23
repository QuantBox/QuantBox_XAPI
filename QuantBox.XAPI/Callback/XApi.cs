using QuantBox.XAPI.Interface;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;

namespace QuantBox.XAPI.Callback
{
    public partial class XApi : BaseApi, IDisposable, IXApi
    {
        internal XApi(string path1, Queue queue)
            : base(path1, queue)
        {
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

        protected override IntPtr OnRespone(byte type, IntPtr pApi1, IntPtr pApi2, double double1, double double2, IntPtr ptr1, int size1, IntPtr ptr2, int size2, IntPtr ptr3, int size3)
        {
            switch ((ResponeType)type)
            {
                case ResponeType.OnRspQryInstrument:
                    _OnRspQryInstrument(ptr1, size1, double1);
                    break;
                case ResponeType.OnRspQryTradingAccount:
                    _OnRspQryTradingAccount(ptr1, size1, double1);
                    break;
                case ResponeType.OnRspQryInvestorPosition:
                    _OnRspQryInvestorPosition(ptr1, size1, double1);
                    break;
                case ResponeType.OnRspQrySettlementInfo:
                    _OnRspQrySettlementInfo(ptr1, size1, double1);
                    break;
                case ResponeType.OnRtnOrder:
                    _OnRtnOrder(ptr1, size1);
                    break;
                case ResponeType.OnRtnTrade:
                    _OnRtnTrade(ptr1, size1);
                    break;
                case ResponeType.OnRtnQuote:
                    _OnRtnQuote(ptr1, size1);
                    break;

                case ResponeType.OnRtnDepthMarketData:
                    _OnRtnDepthMarketData(ptr1);
                    break;
                case ResponeType.OnRtnQuoteRequest:
                    _OnRtnQuoteRequest(ptr1, size1);
                    break;

                case ResponeType.OnRspQryHistoricalTicks:
                    _OnRspQryHistoricalTicks(ptr1, size1, ptr2, size2, double1);
                    break;
                case ResponeType.OnRspQryHistoricalBars:
                    _OnRspQryHistoricalBars(ptr1, size1, ptr2, size2, double1);
                    break;

                default:
                    base.OnRespone(type, pApi1, pApi2, double1, double2, ptr1, size1, ptr2, size2, ptr3, size3);
                    break;
            }

            return IntPtr.Zero;
        }
    }
}
