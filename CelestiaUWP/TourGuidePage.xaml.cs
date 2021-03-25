using CelestiaComponent;
using CelestiaUWP.Helper;
using System.Collections.ObjectModel;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Navigation;

namespace CelestiaUWP
{
    public sealed partial class TourGuidePage : Page
    {
        private CelestiaAppCore mAppCore;
        ObservableCollection<CelestiaDestination> mDestinations = new ObservableCollection<CelestiaDestination>();

        public TourGuidePage()
        {
            this.InitializeComponent();
            TitleText.Text = LocalizationHelper.Localize("Select your destination:");
            GoButton.Content = LocalizationHelper.Localize("Go");
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
