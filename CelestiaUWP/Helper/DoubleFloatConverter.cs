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
