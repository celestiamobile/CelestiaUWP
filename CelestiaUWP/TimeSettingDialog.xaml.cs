//
// TimeSettingDialog.xaml.cs
//
// Copyright © 2021 Celestia Development Team. All rights reserved.
//
// This program is free software, you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation, either version 2
// of the License, or (at your option) any later version.
//

using CelestiaAppComponent;
using System;
using System.ComponentModel;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;

namespace CelestiaUWP
{
    public sealed partial class TimeSettingDialog : ContentDialog, INotifyPropertyChanged
    {
        public DateTimeOffset DisplayDate
        {
            get => Date.Date.AddMinutes(Time.TotalMinutes);
            set
            {
                Date = value.Date;
                var time = value.TimeOfDay;
                Time = new TimeSpan(time.Hours, time.Minutes, 0);
            }
        }

        private DateTimeOffset mDate;
        private TimeSpan mTime;

        private DateTimeOffset Date
        {
            get => mDate;
            set
            {
                mDate = value;
                OnPropertyChanged("mDate");
            }
        }
        private TimeSpan Time
        {
            get => mTime;
            set
            {
                mTime = value;
                OnPropertyChanged("mTime");
            }
        }

        public TimeSettingDialog(DateTimeOffset original)
        {
            this.InitializeComponent();
            DatePicker.MaxYear = new DateTimeOffset(new DateTime(9999, 12, 30));
            DatePicker.MinYear = new DateTimeOffset(new DateTime(1, 1, 2));
            Title = LocalizationHelper.Localize("Set Time");
            PrimaryButtonText = LocalizationHelper.Localize("OK");
            SecondaryButtonText = LocalizationHelper.Localize("Cancel");
            CurrentTimeButton.Content = LocalizationHelper.Localize("Set to Current Time");
            DisplayDate = original;
        }

        public event PropertyChangedEventHandler PropertyChanged;
        private void OnPropertyChanged(string propertyName)
        {
            if (PropertyChanged != null)
            {
                PropertyChanged(this, new PropertyChangedEventArgs(propertyName));
            }
        }

        private void CurrentTimeButton_Click(object sender, RoutedEventArgs e)
        {
            DisplayDate = DateTime.Now;
        }
    }
}
