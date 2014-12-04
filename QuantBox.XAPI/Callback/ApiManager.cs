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

        private static ConcurrentDictionary<XApi, Queue> dict = new ConcurrentDictionary<XApi, Queue>();

        public static XApi CreateApi(string path)
        {
            lock (locker)
            {
                Queue queue = new Queue(QueuePath);
                XApi api = new XApi(path, new Queue(QueuePath));
                dict.TryAdd(api, queue);
                return api;
            }
        }

        public static void ReleaseApi(BaseApi api)
        {
            lock(locker)
            {
                Queue queue;
                if (dict.TryRemove(api as XApi, out queue))
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
