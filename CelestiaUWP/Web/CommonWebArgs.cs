//
// CommonWebArgs.cs
//
// Copyright © 2021 Celestia Development Team. All rights reserved.
//
// This program is free software, you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation, either version 2
// of the License, or (at your option) any later version.
//

using CelestiaComponent;
using System;

namespace CelestiaUWP.Web
{
    public class CommonWebArgs
    {
        public CelestiaAppCore AppCore;
        public CelestiaRenderer Renderer;
        public Uri Uri;
        public string[] MatchingQueryKeys;
        public string ContextDirectory;
        public SendACK ACKReceiver;
    }
}
