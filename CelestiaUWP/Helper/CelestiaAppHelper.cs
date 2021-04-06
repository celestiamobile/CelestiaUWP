//
// CelestiaAppHelper.cs
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
    public class CelestiaAppHelper
    {
        public static string GetInfoURL(CelestiaSelection selection)
        {
            var obj = selection.Object;
            if (obj == null) return null;
            if (obj is CelestiaStar star)
                return star.InfoURL;
            if (obj is CelestiaBody body)
                return body.InfoURL;
            if (obj is CelestiaDSO DSO)
                return DSO.InfoURL;
            return null;
        }
    }
}
