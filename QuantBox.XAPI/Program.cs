using QuantBox.XAPI.Callback;
using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.IO;

namespace QuantBox.XAPI
{
    class Program
    {
        static void OnConnectionStatus(object sender, ConnectionStatus status, ref RspUserLoginField userLogin, int size1)
        {
            Console.WriteLine("11111" + status);
        }

        static void OnConnectionStatus2(object sender, ConnectionStatus status, ref RspUserLoginField userLogin, int size1)
        {
            Console.WriteLine("22222" + status+userLogin.ErrorMsg());
        }

        static void OnRtnDepthMarketData(object sender, ref DepthMarketDataField marketData)
        {
            Console.WriteLine(marketData.InstrumentID);
            Console.WriteLine(marketData.ExchangeID);
            Console.WriteLine(marketData.LastPrice);
        }

        static void OnRspQryInstrument(object sender, ref InstrumentField instrument,int size1, bool bIsLast)
        {
            Console.WriteLine(instrument.InstrumentName());
        }

        static void OnRspQryTradingAccount(object sender, ref AccountField account, int size1, bool bIsLast)
        {

        }

        static void OnRspQrySettlementInfo(object sender, ref SettlementInfoField settlementInfo, int size1, bool bIsLast)
        {

        }

        static void OnRtnOrder(object sender, ref OrderField order)
        {
            Console.WriteLine("AA " + order.Status + order.ExecType + order.Text() + "AA " + order.ID + "AA " + order.OrderID);
        }

        static void OnRtnError(object sender, ref ErrorField error)
        {
            Console.WriteLine("BB" + error.ErrorID + error.ErrorMsg());
        }

        static void OnRtnTrade(object sender, ref TradeField trade)
        {
            Console.WriteLine("CC " + trade.Time + "CC " + trade.ID + "CC " + trade.TradeID);
        }

        static void Main(string[] args)
        {
            UIntPtr a = new UIntPtr(456);
            Console.WriteLine(a);
            return;
            test_CTP_Main(args);
        }

        static void test_Linux_Main(string[] args)
        {
            //Console.WriteLine (Path.GetTempPath());
            //return;
            Queue queue = new Queue(@"libQuantBox_Queue.so");
            //Queue queue2 = new Queue(@"libQuantBox_Queue.so");
            MarketDataApi api = new MarketDataApi(@"/home/hetao/works/QuantBox_X/QuantBox.XAPI/bin/libQuantBox_CTP_Quote.so", queue);
            //TraderApi api2 = new TraderApi(@"C:\Program Files\SmartQuant Ltd\OpenQuant 2014\TAPI\CTP\QuantBox.C2CTP.Trade.dll", queue2);

            api.Server.BrokerID = "1017";
            api.Server.Address = "tcp://ctpmn1-front1.citicsf.com:51213";

            api.User.UserID = "00000015";
            api.User.Password = "123456";

            api.OnConnectionStatus = OnConnectionStatus;
            api.OnRtnDepthMarketData = OnRtnDepthMarketData;

            /*api2.Server.BrokerID = "1017";
            api2.Server.Address = "tcp://ctpmn1-front1.citicsf.com:51205";
            api2.Server.ResumeType = ResumeType.Restart;

            api2.User.UserID = "00000015";
            api2.User.Password = "123456";

            api2.OnConnectionStatus = OnConnectionStatus2;
            api2.OnRspQryInstrument = OnRspQryInstrument;
            api2.OnRspQryTradingAccount = OnRspQryTradingAccount;
            api2.OnRspQrySettlementInfo = OnRspQrySettlementInfo;
            api2.OnRtnOrder = OnRtnOrder;
            api2.OnRtnError = OnRtnError;
            api2.OnRtnTrade = OnRtnTrade;*/

            api.Connect();
            //api2.Connect();

            api.Subscribe("IF1410", "");

            Console.ReadKey();

            Console.ReadKey();

            api.Dispose();
        }

		#region LTS
        static void test_LTS_Main(string[] args)
        {
            Queue queue = new Queue(@"QuantBox_Queue.dll");
            Queue queue2 = new Queue(@"QuantBox_Queue.dll");
            MarketDataApi api = new MarketDataApi("QuantBox_LTS_Quote.dll", queue);
            TraderApi api2 = new TraderApi("QuantBox_C2LTS_Trade.dll", queue2);

            api.Server.BrokerID = "2010";
            api.Server.Address = "tcp://211.144.195.163:44513";

            api.User.UserID = "00000015";
            api.User.Password = "123456";

            api.OnConnectionStatus = OnConnectionStatus;
            api.OnRtnDepthMarketData = OnRtnDepthMarketData;

            api2.Server.BrokerID = "2010";
            api2.Server.Address = "tcp://211.144.195.163:44505";

            api2.User.UserID = "0020090001134";
            api2.User.Password = "123321";

            api2.OnConnectionStatus = OnConnectionStatus2;
            api2.OnRspQryInstrument = OnRspQryInstrument;
            api2.OnRspQryTradingAccount = OnRspQryTradingAccount;
            api2.OnRspQrySettlementInfo = OnRspQrySettlementInfo;
            api2.OnRtnOrder = OnRtnOrder;
            api2.OnRtnError = OnRtnError;
            api2.OnRtnTrade = OnRtnTrade;

            api.Connect();
            api2.Connect();

            Console.ReadKey();

            api2.ReqQryInstrument("", "");

            Console.ReadKey();

            api.Dispose();
            api2.Dispose();
            //queue.Dispose();
        }
		#endregion

        static MarketDataApi api;
        static void test_CTP_Main(string[] args)
        {
            ApiManager.QueuePath = @"C:\Program Files\SmartQuant Ltd\OpenQuant 2014\QuantBox_Queue.dll";
            api = ApiManager.CreateMarketDataApi(@"C:\Program Files\SmartQuant Ltd\OpenQuant 2014\XAPI\CTP\QuantBox_CTP_Quote.dll");

            api.Server.BrokerID = "1017";
            api.Server.Address = "tcp://ctpmn1-front1.citicsf.com:51213";

            api.User.UserID = "00000015";
            api.User.Password = "123456";

            api.OnConnectionStatus = OnConnectionStatus;
            api.OnRtnDepthMarketData = OnRtnDepthMarketData;

            api.Connect();

            api.Subscribe("IF1411", "");

            Console.ReadKey();

            Console.ReadKey();

            ApiManager.Release(api);

            Console.ReadKey();

            Console.ReadKey();

            //api.Dispose();
            //queue.Dispose();
        }
    }
}
