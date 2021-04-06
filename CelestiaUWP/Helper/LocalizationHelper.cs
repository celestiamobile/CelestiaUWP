//
// LocalizationHelper.cs
//
// Copyright © 2021 Celestia Development Team. All rights reserved.
//
// This program is free software, you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation, either version 2
// of the License, or (at your option) any later version.
//

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
