using CelestiaComponent;
using CelestiaUWP.Helper;
using System;
using System.ComponentModel;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Navigation;

namespace CelestiaUWP
{
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
            GoButton.Content = LocalizationHelper.Localize("Set Date and Go to Planet");
            ComputeButton.Content = LocalizationHelper.Localize("Compute");
            StartTimeHint.Text = LocalizationHelper.Localize("Start time:");
            EndTimeHint.Text = LocalizationHelper.Localize("End time:");
            ObjectHint.Text = LocalizationHelper.Localize("Object:");
            SolarEclipseButton.Content = LocalizationHelper.Localize("Solar");
            SolarEclipseButton.Content = LocalizationHelper.Localize("Lunar");
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
