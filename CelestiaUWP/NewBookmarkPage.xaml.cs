//
// NewBookmarkPage.xaml.cs
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
using System.Collections.ObjectModel;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Navigation;

namespace CelestiaUWP
{
    public class NewBookmarkParameter
    {
        public CelestiaAppCore AppCore;
        public CelestiaRenderer Renderer;

        public NewBookmarkParameter(CelestiaAppCore appCore, CelestiaRenderer renderer)
        {
            this.AppCore = appCore;
            this.Renderer = renderer;
        }
    }

    public sealed partial class NewBookmarkPage : Page
    {
        private string NameText;
        private CelestiaAppCore AppCore;
        private CelestiaRenderer Renderer;

        public NewBookmarkPage()
        {
            this.InitializeComponent();

            ConfirmButton.Content = LocalizationHelper.Localize("OK", "");
        }

        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
            var parameter = (NewBookmarkParameter)e.Parameter;
            AppCore = parameter.AppCore;
            Renderer= parameter.Renderer;
            Organizer.Navigate(typeof(BookmarkOrganizerPage), new BookmarkOrganizerParameter(AppCore, Renderer, true));
        }

        private void ConfirmButton_Click(object sender, RoutedEventArgs e)
        {
            if (NameText == null || Renderer == null || AppCore == null) return;
            var name = NameText;
            Renderer.EnqueueTask(() =>
            {
                var url = AppCore.CurrentURL;
                _ = Dispatcher.TryRunAsync(Windows.UI.Core.CoreDispatcherPriority.Normal, () =>
                {
                    var bookmark = new BookmarkNode(false, NameText, AppCore.CurrentURL, BookmarkHelper.CreateEmptyList());
                    var organizerPage = Organizer.Content as BookmarkOrganizerPage;
                    if (organizerPage != null)
                        organizerPage.InsertBookmarkAtSelection(bookmark);
                });
            });
        }
    }
}
