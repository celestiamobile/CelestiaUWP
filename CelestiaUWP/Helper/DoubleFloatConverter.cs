//
// DoubleFloatConverter.cs
//
// Copyright © 2021 Celestia Development Team. All rights reserved.
//
// This program is free software, you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation, either version 2
// of the License, or (at your option) any later version.
//

using System;
using Windows.UI.Xaml.Data;

namespace CelestiaUWP.Helper
{
    public class DoubleFloatConverter : IValueConverter
    {
        public object Convert(object value, Type targetType, object parameter, string language)
        {
            return (double)((float)value);
        }

        public object ConvertBack(object value, Type targetType, object parameter, string language)
        {
            return (float)((double)value);
        }
    }
}
