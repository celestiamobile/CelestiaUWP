using Newtonsoft.Json;

namespace CelestiaUWP.Addon
{
    public class RequestResult
    {
        public struct Info
        {
            public string detail;
            public string reason;
        }

        public int status;
        public Info info;

        public T Get<T>()
        {
            return JsonConvert.DeserializeObject<T>(info.detail);
        }
    }
}
