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
using CelestiaComponent;
using System;
using System.ComponentModel;
using Windows.Globalization.NumberFormatting;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;

namespace CelestiaUWP
{
    public sealed partial class TimeSettingDialog : ContentDialog, INotifyPropertyChanged
    {
        public double JulianDay
        {
            get
            {
                var selectedIndex = TypeSelection.SelectedIndex;
                if (selectedIndex == 0)
                    return CelestiaHelper.JulianDayFromDateTime(Date.Date.AddMinutes(Time.TotalMinutes));
                if (selectedIndex == 1)
                    return julianDay;
                return 0.0;
            }
        }

        private DateTimeOffset PickerDate;
        private TimeSpan PickerTime;
        private double julianDay;

        private readonly DecimalFormatter NumberFormatter = new DecimalFormatter();

        private DateTimeOffset Date
        {
            get => PickerDate;
            set
            {
                PickerDate = value;
                ValidateTime();
            }
        }
        private TimeSpan Time
        {
            get => PickerTime;
            set
            {
                PickerTime = value;
                ValidateTime();
            }
        }

        public TimeSettingDialog(double julianDay)
        {
            NumberFormatter.FractionDigits = 0;
            NumberFormatter.IsGrouped = false;
            this.julianDay = julianDay;
            this.InitializeComponent();
            TypeSelection.ItemsSource = new string[] { LocalizationHelper.Localize("Picker"), LocalizationHelper.Localize("Julian Day") };
            DatePicker.MaxYear = new DateTimeOffset(new DateTime(9999, 12, 30));
            DatePicker.MinYear = new DateTimeOffset(new DateTime(1, 1, 2));
            Title = LocalizationHelper.Localize("Set Time");
            PrimaryButtonText = LocalizationHelper.Localize("OK");
            SecondaryButtonText = LocalizationHelper.Localize("Cancel");
            CurrentTimeButton.Content = LocalizationHelper.Localize("Set to Current Time");
            JulianDayInput.Text = NumberFormatter.FormatDouble(Math.Round(julianDay, 4));
            if (julianDay < CelestiaHelper.MinRepresentableJulianDay() || julianDay > CelestiaHelper.MaxRepresentableJulianDay())
            {
                SetDisplayDate(DateTime.Now);
                TypeSelection.SelectedIndex = 1;
            }
            else
            {
                SetDisplayDate(CelestiaHelper.DateTimeFromJulianDay(julianDay));
                TypeSelection.SelectedIndex = 0;
            }
        }

        public event PropertyChangedEventHandler PropertyChanged;
        private void OnPropertyChanged(string propertyName)
        {
            if (PropertyChanged != null)
            {
                PropertyChanged(this, new PropertyChangedEventArgs(propertyName));
            }
        }

        private void TypeSelection_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            var selectedIndex = TypeSelection.SelectedIndex;
            if (selectedIndex == 0)
            {
                PickerPanel.Visibility = Visibility.Visible;
                JulianDayPanel.Visibility = Visibility.Collapsed;
                ValidateTime();
            }
            else if (selectedIndex == 1)
            {
                PickerPanel.Visibility = Visibility.Collapsed;
                JulianDayPanel.Visibility = Visibility.Visible;
                ValidateJulianDay();
            }
            else
            {
                PickerPanel.Visibility = Visibility.Collapsed;
                JulianDayPanel.Visibility = Visibility.Collapsed;
            }
        }

        private void SetDisplayDate(DateTimeOffset value)
        {
            try
            {
                Date = value.Date;
                var time = value.TimeOfDay;
                Time = new TimeSpan(time.Hours, time.Minutes, 0);
                OnPropertyChanged("Date");
                OnPropertyChanged("Time");
            }
            catch
            {
                SetDisplayDate(DateTime.Now);
            }
        }

        private void CurrentTimeButton_Click(object sender, RoutedEventArgs e)
        {
            SetDisplayDate(DateTime.Now);
        }

        private void ValidateTime()
        {
            if (TypeSelection.SelectedIndex != 0) return;
            try
            {
                DateTimeOffset time = PickerDate.Date.AddMinutes(PickerTime.TotalMinutes);
                IsPrimaryButtonEnabled = true;
                ErrorText.Visibility = Visibility.Collapsed;
            }
            catch
            {
                IsPrimaryButtonEnabled = false;
                ErrorText.Visibility= Visibility.Visible;
                ErrorText.Text = LocalizationHelper.Localize("Selected time is out of range.");
            }
        }

        private void ValidateJulianDay()
        {
            if (TypeSelection.SelectedIndex != 1) return;
            var day = NumberFormatter.ParseDouble(JulianDayInput.Text);
            if (day == null)
            {
                IsPrimaryButtonEnabled = false;
                JulianDayErrorText.Visibility = Visibility.Visible;
                JulianDayErrorText.Text = LocalizationHelper.Localize("Incorrect Julian day string.");
            }
            else
            {
                julianDay = (double)day;
                IsPrimaryButtonEnabled = true;
                JulianDayErrorText.Visibility = Visibility.Collapsed;
            }
        }

        private void JulianDayInput_TextChanged(object sender, TextChangedEventArgs e)
        {
            ValidateJulianDay();
        }
    }
}
