//
// RequestResult.cs
//
// Copyright © 2021 Celestia Development Team. All rights reserved.
//
// This program is free software, you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation, either version 2
// of the License, or (at your option) any later version.
//

using Newtonsoft.Json;

namespace CelestiaUWP.Addon
{
    public class RequestResult
    {
        public struct Info
        {
            public string detail;
            public string reason;
        }

        public int status;
        public Info info;

        public T Get<T>()
        {
            return JsonConvert.DeserializeObject<T>(info.detail);
        }
    }
}
