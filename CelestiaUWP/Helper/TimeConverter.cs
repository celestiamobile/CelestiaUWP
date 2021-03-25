using System;
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
}
