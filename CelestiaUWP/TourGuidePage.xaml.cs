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
using System.Collections.ObjectModel;

// The Blank Page item template is documented at https://go.microsoft.com/fwlink/?LinkId=234238

namespace CelestiaUWP
{
    public sealed partial class TourGuidePage : Page
    {
        private CelestiaAppCore mAppCore;
        ObservableCollection<CelestiaDestination> mDestinations = new ObservableCollection<CelestiaDestination>();

        public TourGuidePage()
        {
            this.InitializeComponent();
            TitleText.Text = CelestiaAppCore.LocalizedString("Select your destination:");
            GoButton.Content = CelestiaAppCore.LocalizedString("Go");
        }

        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
            mAppCore = e.Parameter as CelestiaAppCore;
            mDestinations = new ObservableCollection<CelestiaDestination>(mAppCore.Destinations);
        }

        private void GoButton_Click(object sender, RoutedEventArgs e)
        {
            int index = DestinationSelection.SelectedIndex;
            if (index < 0) return;
            mAppCore.Simulation.GoToDestination(mDestinations[index]);
        }
    }
}
