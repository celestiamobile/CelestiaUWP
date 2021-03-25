using CelestiaUWP.Helper;
using System.ComponentModel;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Navigation;

namespace CelestiaUWP.Addon
{
    public delegate void ShowCategoryHandler(ResourceCategory category);
    public delegate void ShowItemHandler(ResourceItem category);

    public class CategoryParameter
    {
        public ResourceCategory Category;
        public ShowItemHandler Handler;
        public CategoryParameter(ResourceCategory category, ShowItemHandler handler)
        {
            this.Category = category;
            this.Handler = handler;
        }
    }

    public class InstalledListParameter
    {
        public ShowItemHandler Handler;
        public InstalledListParameter(ShowItemHandler handler)
        {
            this.Handler = handler;
        }
    }

    public class ItemParameter
    {
        public ResourceItem Item;
        public ItemParameter(ResourceItem item)
        {
            this.Item = item;
        }
    }

    public sealed partial class ResourceManagerPage : Page, INotifyPropertyChanged
    {
        private Helper.NavigationViewItem[] NavigationItems = new Helper.NavigationViewItem[]
{
            new Helper.NavigationViewItem(LocalizationHelper.Localize("Installed"), "installed"),
        };

        public ResourceManagerPage()
        {
            this.InitializeComponent();
        }

        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
            ResourceManager.Shared.AddonFolderPath = e.Parameter as string;

            Container.Navigate(typeof(ResourceCategoryListPage), (ShowCategoryHandler)delegate (ResourceCategory category)
            {
                ShowCategory(category);
            });
        }

        private void ShowCategory(ResourceCategory category)
        {
            Nav.IsBackEnabled = true;
            Container.Navigate(typeof(ResourceItemListPage), new CategoryParameter(category, (ShowItemHandler)delegate (ResourceItem item)
            {
                ShowItem(item);
            }));
        }

        private void ShowItem(ResourceItem item)
        {
            Nav.IsBackEnabled = true;
            Container.Navigate(typeof(ResourceItemPage), new ItemParameter(item));
        }

        private void ShowInstalledItems()
        {
            Nav.IsBackEnabled = true;
            Container.Navigate(typeof(InstalledItemListPage), new InstalledListParameter((ShowItemHandler)delegate (ResourceItem item)
            {
                ShowItem(item);
            }));
        }


        public event PropertyChangedEventHandler PropertyChanged;
        private void OnPropertyChanged(string propertyName)
        {
            if (PropertyChanged != null)
            {
                PropertyChanged(this, new PropertyChangedEventArgs(propertyName));
            }
        }

        private void Nav_BackRequested(NavigationView sender, NavigationViewBackRequestedEventArgs args)
        {
            if (!Container.CanGoBack) return;

            if (Container.Content is InstalledItemListPage)
                Nav.SelectedItem = null;

            Container.GoBack();
            Nav.IsBackEnabled = Container.CanGoBack;
        }

        private void Nav_SelectionChanged(NavigationView sender, NavigationViewSelectionChangedEventArgs args)
        {
            if (args.SelectedItem != null)
            {
                ShowInstalledItems();
            }
        }
    }
}
