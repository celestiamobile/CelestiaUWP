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
        public string ConfigFileToken;
        public string DataDirToken;

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

            var configFile = settings.Values["ConfigFile"];
            if (configFile is string)
                ConfigFileToken = (string)configFile;

            var dataDir = settings.Values["DataDir"];
            if (dataDir is string)
                DataDirToken = (string)dataDir;
        }

        public void Save()
        {
            var settings = Windows.Storage.ApplicationData.Current.LocalSettings;
            settings.Values["FullDPI"] = UseFullDPI;
            settings.Values["EnableMSAA"] = EnableMSAA;
            settings.Values["ConfigFile"] = ConfigFileToken;
            settings.Values["DataDir"] = DataDirToken;
        }
    }
}
