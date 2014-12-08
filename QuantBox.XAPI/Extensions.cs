using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace QuantBox.XAPI
{
    public static class Extensions_GBK
    {
        public static string InstrumentName(this InstrumentField field)
        {
            return PInvokeUtility.GetUnicodeString(field.InstrumentName);
        }

        public static string Content(this SettlementInfoField field)
        {
            return PInvokeUtility.GetUnicodeString(field.Content);
        }

        public static string Text(this OrderField field)
        {
            return PInvokeUtility.GetUnicodeString(field.Text);
        }

        public static string Text(this QuoteField field)
        {
            return PInvokeUtility.GetUnicodeString(field.Text);
        }

        public static string ErrorMsg(this RspUserLoginField field)
        {
            return PInvokeUtility.GetUnicodeString(field.ErrorMsg);
        }

        public static string ErrorMsg(this ErrorField field)
        {
            return PInvokeUtility.GetUnicodeString(field.ErrorMsg);
        }
    }

    public static class Extensions_Misc
    {
        public static DateTime ExchangeDateTime(this DepthMarketDataField field)
        {
            int yyyy = field.ActionDay / 10000;
            int MM = field.ActionDay % 10000 / 100;
            int dd = field.ActionDay % 100;

            int HH = field.UpdateTime / 10000;
            int mm = field.UpdateTime % 10000 / 100;
            int ss = field.UpdateTime % 100;

            return new DateTime(yyyy, MM, dd, HH, mm, ss, field.UpdateMillisec);
        }

        public static DateTime ExchangeDateTime_(this DepthMarketDataField field)
        {
            // 表示传回来的时间可能有问题，要检查一下
            if(field.UpdateTime == 0)
            {
                DateTime now = DateTime.Now;

                int HH = now.Hour;
                int mm = now.Minute;
                int ss = now.Second;
                int datetime = HH * 10000 + mm * 100 + ss;

                if (datetime > 1500 && datetime < 234500)
                    return now;
            }

            {
                int HH = field.UpdateTime / 10000;
                int mm = field.UpdateTime % 10000 / 100;
                int ss = field.UpdateTime % 100;

                DateTime now = DateTime.Now;
                if (HH >= 23)
                {
                    if (now.Hour < 1)
                    {
                        // 表示行情时间慢了，系统日期减一天即可
                        now = now.AddDays(-1);
                    }
                }
                else if (HH < 1)
                {
                    if (now.Hour >= 23)
                    {
                        // 表示本地时间慢了，本地时间加一天即可
                        now = now.AddDays(1);
                    }
                }

                return now.Date.AddSeconds(HH * 3600 + mm * 60 + ss).AddMilliseconds(field.UpdateMillisec);
            }
        }
    }

    public static class Extensions_Output
    {
        public static string ToFormattedString(this ErrorField field)
        {
            return string.Format("[ErrorID={0},ErrorMsg={1}]",
                field.ErrorID, field.ErrorMsg());
        }

        public static string ToFormattedString(this OrderField field)
        {
            return string.Format("[InstrumentID={0};ExchangeID={1};Side={2};Qty={3};Price={4};OpenClose={5};HedgeFlag={6};"
                + "ID={7};OrderID={8};"
                + "Type={9};TimeInForce={10};Status={11};ExecType={12};"
                + "ErrorID={13};Text={14}]",
                field.InstrumentID, field.ExchangeID, field.Side, field.Qty, field.Price, field.OpenClose, field.HedgeFlag,
                field.ID, field.OrderID,
                field.Type, field.TimeInForce, field.Status, field.ExecType,
                field.ErrorID, field.Text());
        }

        public static string ToFormattedString(this TradeField field)
        {
            return string.Format("[InstrumentID={0};ExchangeID={1};Side={2};Qty={3};Price={4};OpenClose={5};HedgeFlag={6};"
                + "ID={7};TradeID={8};"
                + "Time={9};Commission={10}]",
                field.InstrumentID, field.ExchangeID, field.Side, field.Qty, field.Price, field.OpenClose, field.HedgeFlag,
                field.ID, field.TradeID,
                field.Time, field.Commission);
        }

        public static string ToFormattedString(this QuoteField field)
        {
            return string.Format("[InstrumentID={0};ExchangeID={1};"
                + "AskPrice={2};AskQty={3};BidPrice={4};BidQty={5};"
                + "ID={6};AskOrderID={7};BidOrderID={8};"
                + "Status={9};ExecType={10};"
                + "ErrorID={11};Text={12};"
                + "AskOpenClose={13};AskHedgeFlag={14};BidOpenClose={15};BidHedgeFlag={16}]",
                field.InstrumentID, field.ExchangeID,
                field.AskPrice, field.AskQty, field.BidPrice, field.BidQty,
                field.ID,field.AskOrderID,field.BidOrderID,
                field.Status,field.ExecType,
                field.ErrorID,field.Text(),
                field.AskOpenClose,field.AskHedgeFlag,field.BidOpenClose,field.BidHedgeFlag);
        }

        public static string ToFormattedString(this RspUserLoginField field)
        {
            return string.Format("[TradingDay={0};LoginTime={1};SessionID={2};ErrorID={3};ErrorMsg={4}]",
                field.TradingDay,field.LoginTime,field.SessionID,field.ErrorID,field.ErrorMsg());
        }

        public static string ToFormattedString(this QuoteRequestField field)
        {
            return string.Format("[TradingDay={0};InstrumentID={1};ExchangeID={2};QuoteID={3};QuoteTime={4}]",
                field.TradingDay, field.InstrumentID, field.ExchangeID, field.QuoteID, field.QuoteTime);
        }

        public static string ToFormattedHeader(this TickField field)
        {
            return "DateTime,Price,Size,OpenInt,Bid,BidSize,Ask,AskSize";
        }

        public static string ToFormattedString(this TickField field)
        {
            int yyyy = field.Date / 10000;
            int MM = field.Date % 10000 / 100;
            int dd = field.Date % 100;

            int hh = field.Time / 10000;
            int mm = field.Time % 10000 / 100;
            int ss = field.Time % 100;

            string date = string.Format("{0}-{1:D2}-{2:D2} {3:D2}:{4:D2}:{5:D2}.{6:D3}", yyyy, MM, dd, hh, mm, ss, field.Millisecond);
            return string.Format("{0},{1},{2},{3},{4},{5},{6},{7}",
                date, field.LastPrice, field.Volume, field.OpenInterest,
                field.BidPrice1,field.BidSize1,field.AskPrice1,field.AskSize1);
        }

        public static string ToFormattedHeader(this BarField field)
        {
            return "DateTime,Open,High,Low,Close,Volume,OpenInt";
        }

        public static string ToFormattedString(this BarField field)
        {
            int yyyy = field.Date / 10000;
            int MM = field.Date % 10000 / 100;
            int dd = field.Date % 100;

            int hh = field.Time / 10000;
            int mm = field.Time % 10000 / 100;
            int ss = field.Time % 100;

            string date = string.Format("{0}-{1:D2}-{2:D2} {3:D2}:{4:D2}:{5:D2}.000", yyyy, MM, dd, hh, mm, ss);
            return string.Format("{0},{1},{2},{3},{4},{5},{6}",
                date, field.Open, field.High, field.Low, field.Close, field.Volume, field.OpenInterest);
        }
    }
}
