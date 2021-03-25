using CelestiaComponent;

namespace CelestiaUWP.Helper
{
    public class CelestiaAppHelper
    {
        public static string GetInfoURL(CelestiaSelection selection)
        {
            var obj = selection.Object;
            if (obj == null) return null;
            if (obj is CelestiaStar)
                return ((CelestiaStar)obj).InfoURL;
            if (obj is CelestiaBody)
                return ((CelestiaBody)obj).InfoURL;
            if (obj is CelestiaDSO)
                return ((CelestiaDSO)obj).InfoURL;
            return null;
        }
    }
}
