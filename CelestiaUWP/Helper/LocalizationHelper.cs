using CelestiaComponent;

namespace CelestiaUWP.Helper
{


    public class LocalizationHelper
    {
        public static string Locale = "en";
        public static string Localize(string original)
        {
            return CelestiaAppCore.LocalizedString(original, "celestia_ui");
        }
    }
}
