//
// TimeConverter.cs
//
// Copyright © 2021 Celestia Development Team. All rights reserved.
//
// This program is free software, you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation, either version 2
// of the License, or (at your option) any later version.
//

using System;
using Windows.Globalization.DateTimeFormatting;
using Windows.UI.Xaml.Data;

namespace CelestiaUWP.Helper
{
    public class TimeConverter : IValueConverter
    {
        public object Convert(object value, Type targetType, object parameter, string language)
        {
            if (value == null)
                return null;
            return new DateTimeOffset(((DateTime)value).ToUniversalTime());
        }

        public object ConvertBack(object value, Type targetType, object parameter, string language)
        {
            if (value == null)
                return null;
            return ((DateTimeOffset)value).DateTime;
        }
    }

    public class TimeStringConverter : IValueConverter
    {
        private DateTimeFormatter dateFormatter = new DateTimeFormatter("shortdate shorttime");
        public object Convert(object value, Type targetType, object parameter, string language)
        {
            if (value == null)
                return null;
            return dateFormatter.Format((DateTimeOffset)value);
        }

        public object ConvertBack(object value, Type targetType, object parameter, string language)
        {
            return null;
        }
    }
}
