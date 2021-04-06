//
// AppSettings.cs
//
// Copyright © 2021 Celestia Development Team. All rights reserved.
//
// This program is free software, you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation, either version 2
// of the License, or (at your option) any later version.
//

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
            if (fullDPI is bool boolean && !boolean)
                UseFullDPI = false;
            else
                UseFullDPI = true;
            var msaa = settings.Values["EnableMSAA"];
            EnableMSAA = msaa is bool boolean1 && boolean1;

            var configFile = settings.Values["ConfigFile"];
            if (configFile is string configFileValue)
                ConfigFileToken = configFileValue;

            var dataDir = settings.Values["DataDir"];
            if (dataDir is string dataDirValue)
                DataDirToken = dataDirValue;
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
