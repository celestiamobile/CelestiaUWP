using CelestiaUWP.Helper;
using Newtonsoft.Json;
using System;
using System.ComponentModel;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Data;
using Windows.UI.Xaml.Media.Imaging;
using Windows.UI.Xaml.Navigation;

namespace CelestiaUWP.Addon
{
    public class ImageConverter : IValueConverter
    {
        public object Convert(object value, Type targetType, object parameter, string language)

        {
            if (value == null)
            {
                BitmapImage blankimage = new BitmapImage();
                return blankimage;
            }
            else
            {
                return new BitmapImage(new Uri(value.ToString()));
            }
        }

        public object ConvertBack(object value, Type targetType, object parameter, string language)
        {
            throw new NotImplementedException();
        }
    }

    public sealed partial class ResourceItemPage : Page, INotifyPropertyChanged
    {
        private ResourceItem mItem;
        ResourceItem Item
        {
            get { return mItem; }
            set
            {
                mItem = value;
                OnPropertyChanged("Item");
            }
        }

        ResourceManager.ItemState State
        {
            get
            {
                return ResourceManager.Shared.StateForItem(Item);
            }
        }

        public ResourceItemPage()
        {
            this.InitializeComponent();
        }

        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
            var param = e.Parameter as ItemParameter;
            Item = param.Item;

            ResourceManager.Shared.ProgressUpdate += Shared_ProgressUpdate;
            ResourceManager.Shared.DownloadSuccess += Shared_DownloadSuccess;
            ResourceManager.Shared.DownloadFailure += Shared_DownloadFailure;

            UpdateState();
            ReloadItem();
        }

        private void Shared_DownloadFailure(ResourceItem item)
        {
            UpdateState();
        }

        private void Shared_DownloadSuccess(ResourceItem item)
        {
            UpdateState();
        }

        private void Shared_ProgressUpdate(ResourceItem item, double progress)
        {
            if (item.id != Item.id) return;
            ProgressText.Text = String.Format("{0:0.00}%", progress);
        }

        private void UpdateState()
        {
            switch (State)
            {
                case ResourceManager.ItemState.Downloading:
                    ActionButton.Content = LocalizationHelper.Localize("Cancel");
                    break;
                case ResourceManager.ItemState.Installed:
                    ProgressText.Text = "";
                    ActionButton.Content = LocalizationHelper.Localize("Uninstall");
                    break;
                case ResourceManager.ItemState.None:
                    ProgressText.Text = "";
                    ActionButton.Content = LocalizationHelper.Localize("Install");
                    break;
            }
        }

        private async void ReloadItem()
        {
            Windows.Web.Http.HttpClient httpClient = new Windows.Web.Http.HttpClient();
            Uri requestUri = new Uri(Constants.APIPrefix + "/resource/item?lang=" + LocalizationHelper.Locale + "&item=" + Item.id);
            try
            {
                var httpResponse = await httpClient.GetAsync(requestUri);
                httpResponse.EnsureSuccessStatusCode();
                var httpResponseBody = await httpResponse.Content.ReadAsStringAsync();
                var item = JsonConvert.DeserializeObject<RequestResult>(httpResponseBody).Get<ResourceItem>();
                Item = item;
            }
            catch (Exception ex)
            {
                ContentDialogHelper.ShowAlert(this, ex.Message);
            }
        }

        private void ActionButton_Click(object sender, RoutedEventArgs e)
        {
            switch (State)
            {
                case ResourceManager.ItemState.Downloading:
                    ResourceManager.Shared.Cancel(Item);
                    UpdateState();
                    break;
                case ResourceManager.ItemState.Installed:
                    ResourceManager.Shared.Uninstall(Item);
                    UpdateState();
                    break;
                case ResourceManager.ItemState.None:
                    ResourceManager.Shared.DownloadItem(Item);
                    UpdateState();
                    break;
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
    }
}
