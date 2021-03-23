using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Runtime.InteropServices.WindowsRuntime;
using Windows.Foundation;
using Windows.Foundation.Collections;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Controls.Primitives;
using Windows.UI.Xaml.Data;
using Windows.UI.Xaml.Input;
using Windows.UI.Xaml.Media;
using Windows.UI.Xaml.Navigation;

using CelestiaComponent;
using System.ComponentModel;

namespace CelestiaUWP
{
    public class FilterStarsConverter : IValueConverter
    {
        public object Convert(object value, Type targetType, object parameter, string language)
        {
            return (double)MathF.Log((float)value, 1000000f) * 10000;
        }

        public object ConvertBack(object value, Type targetType, object parameter, string language)
        {
            return (float)MathF.Pow(1000000f, (float)((double)value) / 10000f);
        }
    }
    public sealed partial class ViewOptionsPage : Page, INotifyPropertyChanged
    {
        private string[] InfoDescriptions = new string[]
        {
            "None", "Terse", "Verbose"
        };

        private CelestiaAppCore AppCore
        {
            get { return mAppCore; }
            set
            {
                mAppCore = value;
                OnPropertyChanged("AppCore");
            }
        }

        private CelestiaAppCore mAppCore;

        public ViewOptionsPage()
        {
            this.InitializeComponent();
        }

        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
            mAppCore = e.Parameter as CelestiaAppCore;
        }

        public event PropertyChangedEventHandler PropertyChanged;
        private void OnPropertyChanged(string propertyName)
        {
            if (PropertyChanged != null)
            {
                PropertyChanged(this, new PropertyChangedEventArgs(propertyName));
            }
        }
    }
}
