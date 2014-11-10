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
        private static object locker = new object();

        public static string QueuePath;

        private static ConcurrentDictionary<TraderApi, Queue> dict = new ConcurrentDictionary<TraderApi, Queue>();

        public static TraderApi CreateApi(string path)
        {
            lock (locker)
            {
                Queue queue = new Queue(QueuePath);
                TraderApi api = new TraderApi(path, new Queue(QueuePath));
                dict.TryAdd(api, queue);
                return api;
            }
        }

        public static void ReleaseApi(BaseApi api)
        {
            lock(locker)
            {
                Queue queue;
                if (dict.TryRemove(api as TraderApi, out queue))
                {
                    api.Dispose();
                    queue.Dispose();

                    api = null;
                    queue = null;
                }
            }
        }
    }
}
