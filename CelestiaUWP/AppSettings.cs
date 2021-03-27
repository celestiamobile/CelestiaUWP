using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace CelestiaUWP
{
    public class AppSettings
    {
        public static AppSettings Shared = new AppSettings();

        public bool UseFullDPI;
        public bool EnableMSAA;

        private AppSettings()
        {
            var settings = Windows.Storage.ApplicationData.Current.LocalSettings;
            var fullDPI = settings.Values["FullDPI"];
            if (fullDPI is bool && !(bool)fullDPI)
                UseFullDPI = false;
            else
                UseFullDPI = true;
            var msaa = settings.Values["EnableMSAA"];
            if (msaa is bool && (bool)msaa)
                EnableMSAA = true;
            else
                EnableMSAA = false;
        }

        public void Save()
        {
            var settings = Windows.Storage.ApplicationData.Current.LocalSettings;
            settings.Values["FullDPI"] = UseFullDPI;
            settings.Values["EnableMSAA"] = EnableMSAA;
        }
    }
}
