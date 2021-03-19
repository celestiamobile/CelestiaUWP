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

namespace CelestiaUWP
{
    public sealed partial class TextInputDialog : ContentDialog
    {
        public string Text = "";

        public TextInputDialog(string title)
        {
            this.InitializeComponent();
            Title = title;
            PrimaryButtonText = CelestiaAppCore.LocalizedString("OK");
            SecondaryButtonText = CelestiaAppCore.LocalizedString("Cancel");
        }

        private void ContentDialog_PrimaryButtonClick(ContentDialog sender, ContentDialogButtonClickEventArgs args)
        {
        }

        private void ContentDialog_SecondaryButtonClick(ContentDialog sender, ContentDialogButtonClickEventArgs args)
        {
        }
    }
}
