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
        public string objectName;
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

        public Windows.UI.Xaml.Visibility ReleaseTimeVisibility
        {
            get { return publishTime == 0 ? Windows.UI.Xaml.Visibility.Collapsed : Windows.UI.Xaml.Visibility.Visible; }
        }

        public Uri WebLink
        {
            get
            {
                var queryItems = System.Web.HttpUtility.ParseQueryString("");
                queryItems.Add("lang", LocalizationHelper.Locale);
                queryItems.Add("item", id);
                var builder = new UriBuilder("https://celestia.mobi/resources/item");
                builder.Query = queryItems.ToString();
                return builder.Uri;
            }
        }

        public string ReleaseTimeDescription
        {
            get
            {
                return string.Format(LocalizationHelper.Localize("Release date: %s").Replace("%s", "{0}"), ReleaseTime.ToShortDateString());
            }
        }

        public string AuthorListDescription
        {
            get
            {
                if (authors == null || authors.Length == 0) return null;
                return string.Format(LocalizationHelper.Localize("Authors: %s").Replace("%s", "{0}"), string.Join(", ", authors));
            }
        }
    }
}
