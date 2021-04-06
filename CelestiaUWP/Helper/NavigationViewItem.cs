//
// NavigationViewItem.cs
//
// Copyright © 2021 Celestia Development Team. All rights reserved.
//
// This program is free software, you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation, either version 2
// of the License, or (at your option) any later version.
//

namespace CelestiaUWP.Helper
{
    public class NavigationViewItem
    {
        public string Name;
        public string Tag;

        public NavigationViewItem(string Name, string Tag)
        {
            this.Name = Name;
            this.Tag = Tag;
        }
    }
}
