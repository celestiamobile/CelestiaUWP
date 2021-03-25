using CelestiaUWP.Helper;
using System.ComponentModel;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Navigation;

namespace CelestiaUWP.Addon
{
    public sealed partial class InstalledItemListPage : Page, INotifyPropertyChanged
    {
        private ShowItemHandler Handler;

        private ResourceItem[] mItems = new ResourceItem[] { };

        ResourceItem[] Items
        {
            get { return mItems; }
            set
            {
                mItems = value;
                OnPropertyChanged("Items");
            }
        }


        public InstalledItemListPage()
        {
            this.InitializeComponent();
            Title.Text = LocalizationHelper.Localize("Installed");
        }
        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
            var parameter = e.Parameter as InstalledListParameter;
            Handler = parameter.Handler;
            LoadItems();
        }

        private async void LoadItems()
        {
            Items = await ResourceManager.Shared.InstalledItems();
        }

        public event PropertyChangedEventHandler PropertyChanged;
        private void OnPropertyChanged(string propertyName)
        {
            if (PropertyChanged != null)
            {
                PropertyChanged(this, new PropertyChangedEventArgs(propertyName));
            }
        }

        private void ListView_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            Handler((ResourceItem)ItemList.SelectedItem);
        }
    }
}
