//
// ResourceManager.cs
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
using System.Collections.Generic;
using System.IO;
using System.IO.Compression;
using System.Linq;
using System.Net.Http;
using System.Threading;
using System.Threading.Tasks;
using Windows.Foundation;
using Windows.Storage;

namespace CelestiaUWP.Addon
{
    public delegate void DownloadProgressUpdate(ResourceItem item, double progress);
    public delegate void DownloadSuccessHandler(ResourceItem item);
    public delegate void DownloadFailureHandler(ResourceItem item);

    public class ResourceManager
    {
        public string AddonFolderPath;

        public static ResourceManager Shared = new ResourceManager();

        public event DownloadProgressUpdate ProgressUpdate;
        public event DownloadSuccessHandler DownloadSuccess;
        public event DownloadFailureHandler DownloadFailure;

        private Dictionary<string, CancellationTokenSource> CancellationTokens = new Dictionary<string, CancellationTokenSource>();

        public enum ItemState
        {
            None,
            Downloading,
            Installed,
        };

        public string ItemPath(ResourceItem item)
        {
            return AddonFolderPath + "\\" + item.ID;
        }

        public ItemState StateForItem(ResourceItem item)
        {
            if (CancellationTokens.ContainsKey(item.ID))
                return ItemState.Downloading;
            else if (Directory.Exists(ItemPath(item)))
                return ItemState.Installed;
            else
                return ItemState.None;
        }

        public async void DownloadItem(ResourceItem item)
        {
            var client = new HttpClient();
            var cancellationTokenSource = new CancellationTokenSource();
            var cancellationToken = cancellationTokenSource.Token;
            CancellationTokens[item.ID] = cancellationTokenSource;

            try
            {
                Progress<double> progress = new Progress<double>();

                progress.ProgressChanged += (sender, value) =>
                {
                    ReportProgress(item, value);
                };

                var tempFolder = Windows.Storage.ApplicationData.Current.TemporaryFolder;
                var tempFile = await tempFolder.CreateFileAsync(GuidHelper.CreateNewGuid().ToString() + ".zip");

                var response = await client.GetAsync(item.URL, HttpCompletionOption.ResponseHeadersRead, cancellationToken);
                response.EnsureSuccessStatusCode();

                var total = response.Content.Headers.ContentLength.HasValue ? response.Content.Headers.ContentLength.Value : -1L;
                var canReportProgress = total != -1 && progress != null;

                using (var stream = await response.Content.ReadAsStreamAsync())
                using (var outputStream = await tempFile.OpenStreamForWriteAsync())
                {
                    var totalRead = 0L;
                    var buffer = new byte[4096];
                    var isMoreToRead = true;

                    do
                    {
                        cancellationToken.ThrowIfCancellationRequested();
                        var read = await stream.ReadAsync(buffer, 0, buffer.Length, cancellationToken);

                        if (read == 0)
                        {
                            isMoreToRead = false;
                        }
                        else
                        {
                            var data = new byte[read];
                            buffer.ToList().CopyTo(0, data, 0, read);
                            totalRead += read;

                            await outputStream.WriteAsync(data, 0, read);

                            if (canReportProgress)
                            {
                                ((IProgress<double>)progress).Report((totalRead * 1d) / (total * 1d));
                            }
                        }
                    } while (isMoreToRead);
                }
                var destinationPath = ItemPath(item);
                ZipFile.ExtractToDirectory(tempFile.Path, destinationPath);
                File.WriteAllText(destinationPath + "\\description.json", item.JSONRepresentation.Stringify());

                CancellationTokens.Remove(item.ID);
                DownloadSuccess(item);
            }
            catch
            {
                CancellationTokens.Remove(item.ID);
                DownloadFailure(item);
            }
        }

        private void ReportProgress(ResourceItem item, double progress)
        {
            ProgressUpdate(item, progress);
        }

        public async Task<ResourceItem[]> InstalledItems()
        {
            var subfolders = new List<StorageFolder>();
            var items = new List<ResourceItem>();
            try
            {
                var folder = await StorageFolder.GetFolderFromPathAsync(AddonFolderPath);
                var folders = await folder.GetFoldersAsync();
                foreach (var f in folders)
                {
                    subfolders.Add(f);
                }
            }
            catch { }

            foreach (var subfolder in subfolders)
            {
                try
                {
                    var descriptionFile = await subfolder.GetFileAsync("description.json");
                    var content = await FileIO.ReadTextAsync(descriptionFile);
                    var item = ResourceItem.TryParse(content);
                    if (item != null)
                    {
                        items.Add(item);
                    }
                }
                catch { }
            }

            return items.ToArray();
        }

        public void Cancel(ResourceItem item)
        {
            CancellationTokenSource cancellationToken;
            if (CancellationTokens.Remove(item.ID, out cancellationToken))
                cancellationToken.Cancel();
        }

        public void Uninstall(ResourceItem item)
        {
            try
            {
                Directory.Delete(ItemPath(item), true);
            }
            catch { }
        }
    }
}
