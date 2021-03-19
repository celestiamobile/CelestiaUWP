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

    public sealed partial class EclipseFinderPage : Page, INotifyPropertyChanged
    {
        private CelestiaAppCore mAppCore;

        private DateTime? mStartTime;
        private DateTime? mEndTime;

        private bool mFindSolar = false;
        private bool mFindLunar = false;

        private int mSelectedObjectIndex = 0;
        private string[] mAvailableObjects = new string[] { "Earth", "Jupiter" };
        private CelestiaEclipse[] mEclipses
        {
            get { return eclipses; }
            set
            {
                eclipses = value;
                OnPropertyChanged("mEclipses");
            }
        }

        private CelestiaEclipse[] eclipses = new CelestiaEclipse[] { };

        private int mSelectedEclipseIndex = -1;

        public EclipseFinderPage()
        {
            this.InitializeComponent();
            GoButton.Content = CelestiaAppCore.LocalizedString("Set Date and Go to Planet");
            ComputeButton.Content = CelestiaAppCore.LocalizedString("Compute");
            StartTimeHint.Text = CelestiaAppCore.LocalizedString("Start time:");
            EndTimeHint.Text = CelestiaAppCore.LocalizedString("End time:");
            ObjectHint.Text = CelestiaAppCore.LocalizedString("Object:");
            SolarEclipseButton.Content = CelestiaAppCore.LocalizedString("Solar");
            SolarEclipseButton.Content = CelestiaAppCore.LocalizedString("Lunar");
        }

        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
            mAppCore = e.Parameter as CelestiaAppCore;
        }

        private void ComputeButton_Click(object sender, RoutedEventArgs e)
        {
            if (mStartTime == null || mEndTime == null)
                return;
            var startTime = (DateTime)mStartTime;
            var endTime = (DateTime)mEndTime;
            if (endTime.CompareTo(startTime) <= 0)
                return;

            if (!mFindLunar && !mFindSolar)
                return;

            CelestiaEclipseKind kind;
            if (mFindLunar && mFindSolar)
                kind = CelestiaEclipseKind.solarAndLunar;
            else if (mFindSolar)
                kind = CelestiaEclipseKind.solar;
            else
                kind = CelestiaEclipseKind.lunar;

            var body = mAppCore.Simulation.Find(mAvailableObjects[mSelectedObjectIndex]).Object;
            if (body == null || !(body is CelestiaBody))
                return;

            var eclipseFinder = new CelestiaEclipseFinder((CelestiaBody)body);
            var eclipses = eclipseFinder.Search(kind, startTime, endTime);
            mEclipses = eclipses;
        }

        private void GoButton_Click(object sender, RoutedEventArgs e)
        {
            if (mSelectedEclipseIndex < 0)
                return;

            var eclipse = mEclipses[mSelectedEclipseIndex];
            mAppCore.Simulation.GoToEclipse(eclipse);
        }

        public event PropertyChangedEventHandler PropertyChanged;
        private void OnPropertyChanged(string propertyName)
        {
            if (PropertyChanged != null)
            {
                PropertyChanged(this, new PropertyChangedEventArgs(propertyName));
            }
        }
        static public String GetEclipseDescription(CelestiaBody occulter, CelestiaBody receiver)
        {
            return occulter.Name + " - " + receiver.Name;
        }
    }
}
