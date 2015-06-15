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

        public static DepthMarketDataNClass GetDepthMarketDataNClass(IntPtr ptr)
        {
            DepthMarketDataNField obj = (DepthMarketDataNField)Marshal.PtrToStructure(ptr, typeof(DepthMarketDataNField));

            DepthMarketDataNClass cls = new DepthMarketDataNClass();
            
            //obj.Size;
            cls.TradingDay = obj.TradingDay;
            cls.ActionDay = obj.ActionDay;
            cls.UpdateTime = obj.UpdateTime;
            cls.UpdateMillisec = obj.UpdateMillisec;
            cls.Exchange = obj.Exchange;
            cls.Symbol = obj.Symbol;
            cls.InstrumentID = obj.InstrumentID;
            cls.LastPrice = obj.LastPrice;
            cls.Volume = obj.Volume;
            cls.Turnover = obj.Turnover;
            cls.OpenInterest = obj.OpenInterest;
            cls.AveragePrice = obj.AveragePrice;
            cls.OpenPrice = obj.OpenPrice;
            cls.HighestPrice = obj.HighestPrice;
            cls.LowestPrice = obj.LowestPrice;
            cls.ClosePrice = obj.ClosePrice;
            cls.SettlementPrice = obj.SettlementPrice;
            cls.UpperLimitPrice = obj.UpperLimitPrice;
            cls.LowerLimitPrice = obj.LowerLimitPrice;
            cls.PreSettlementPrice = obj.PreSettlementPrice;
            cls.PreOpenInterest = obj.PreOpenInterest;
            //obj.BidCount;

            int size = Marshal.SizeOf(typeof(DepthField));
            IntPtr pBid = ptr + Marshal.SizeOf(typeof(DepthMarketDataNField));
            int AskCount = (obj.Size - Marshal.SizeOf(typeof(DepthMarketDataNField))) / size - obj.BidCount;
            IntPtr pAsk = ptr + Marshal.SizeOf(typeof(DepthMarketDataNField)) + obj.BidCount * size;

            cls.Bids = new DepthField[obj.BidCount];
            cls.Asks = new DepthField[AskCount];
            
            for (int i = 0; i < obj.BidCount; ++i)
            {
                cls.Bids[i] = (DepthField)Marshal.PtrToStructure(pBid + i * size, typeof(DepthField));
            }

            for (int i = 0; i < AskCount; ++i)
            {
                cls.Asks[i] = (DepthField)Marshal.PtrToStructure(pAsk + i * size, typeof(DepthField));
            }

            return cls;
        }
    }
}
