//
// ResourceItem.cs
//
// Copyright © 2021 Celestia Development Team. All rights reserved.
//
// This program is free software, you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation, either version 2
// of the License, or (at your option) any later version.
//

using CelestiaUWP.Helper;
using System;

namespace CelestiaUWP.Addon
{
    public class ResourceItem
    {
        public string name;
        public string description;
        public string id;
        public string image;
        public string item;
        public string[] authors;
        public long publishTime;

        public DateTime ReleaseTime
        {
            get { return DateTimeOffset.FromUnixTimeSeconds(publishTime).DateTime; }
        }

        public string ReleaseTimeDescription
        {
            get
            {
                return String.Format(LocalizationHelper.Localize("Release date: %s").Replace("%s", "{0}"), ReleaseTime.ToShortDateString());
            }
        }

        public string AuthorListDescription
        {
            get
            {
                if (authors == null || authors.Length == 0) return null;
                return String.Format(LocalizationHelper.Localize("Authors: %s").Replace("%s", "{0}"), string.Join(", ", authors));
            }
        }
    }
}
