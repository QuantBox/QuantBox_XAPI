using System;
using System.Collections.Concurrent;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace QuantBox.XAPI.Callback
{
    public static class ApiManager
    {
        public static string QueuePath;

        private static ConcurrentDictionary<BaseApi, Queue> dict = new ConcurrentDictionary<BaseApi, Queue>();

        public static MarketDataApi CreateMarketDataApi(string path)
        {
            Queue queue = new Queue(QueuePath);
            MarketDataApi api = new MarketDataApi(path, new Queue(QueuePath));
            dict.TryAdd(api, queue);
            return api;
        }

        public static TraderApi CreateTraderApi(string path)
        {
            Queue queue = new Queue(QueuePath);
            TraderApi api = new TraderApi(path, new Queue(QueuePath));
            dict.TryAdd(api, queue);
            return api;
        }

        public static void Release(BaseApi api)
        {
            Queue queue;
            if(dict.TryRemove(api,out queue))
            {
                api.Dispose();
                queue.Dispose();

                api = null;
                queue = null;
            }
        }
    }
}
