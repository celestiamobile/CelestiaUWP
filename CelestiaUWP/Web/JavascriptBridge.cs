//
// JavascriptBridge.cs
//
// Copyright © 2021 Celestia Development Team. All rights reserved.
//
// This program is free software, you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation, either version 2
// of the License, or (at your option) any later version.
//

using CelestiaComponent;
using Newtonsoft.Json;
using System;
using Windows.ApplicationModel.DataTransfer;
using Windows.Foundation;
using Windows.UI.Core;

namespace CelestiaUWP.Web
{
    public interface JavascriptMessageHandler
    {
        string operation { get; }
        void executeWithContent(string content, JavascriptBridge bridge);
    }

    public abstract class TypedJavascriptMessageHandler<T> : JavascriptMessageHandler
    {
        public abstract string operation { get; }

        public void executeWithContent(string content, JavascriptBridge bridge)
        {
            try
            {
                var context = JsonConvert.DeserializeObject<T>(content);
                execute(context, bridge);
            }
            catch { }
        }

        public abstract void execute(T context, JavascriptBridge bridge);
    }

    class MessagePayload
    {
        public string operation;
        public string content;
        public int minScriptVersion;
    }

    class RunScriptContext
    {
        public string scriptContent;
        public string scriptType;
        public string scriptName;
        public string scriptLocation;
    }

    class ShareURLContext
    {
        public string title;
        public string url;
    }

    class SendACKContext
    {
        public string id;
    }

    class DummyContext
    {
    }

    class RunDemoContext
    {
    }

    class ShareURLHandler : TypedJavascriptMessageHandler<ShareURLContext>
    {
        public override string operation => "shareURL";
        private ShareURLContext currentContext;

        public override void execute(ShareURLContext context, JavascriptBridge bridge)
        {
            if (context.url != null && context.title != null)
            {
                shareURL(context, bridge.dispatcher);
            }
        }

        private async void shareURL(ShareURLContext context, CoreDispatcher dispatcher)
        {
            await dispatcher.RunAsync(CoreDispatcherPriority.Normal, () =>
            {
                currentContext = context;
                var transferManager = DataTransferManager.GetForCurrentView();
                transferManager.DataRequested += TransferManager_DataRequested;
                DataTransferManager.ShowShareUI();
            });
        }

        private void TransferManager_DataRequested(DataTransferManager sender, DataRequestedEventArgs args)
        {
            if (currentContext == null) return;
            DataRequest request = args.Request;
            request.Data.Properties.Title = currentContext.title;
            request.Data.SetWebLink(new Uri(currentContext.url));
        }
    }

    class RunScriptHandler : TypedJavascriptMessageHandler<RunScriptContext>
    {
        public override string operation => "runScript";
        private static string[] supportedScriptLocations = new[] { "temp", "context" };
        private static string[] supportedScriptTypes = new[] { "cel", "celx" };

        public override void execute(RunScriptContext context, JavascriptBridge bridge)
        {
            if (context.scriptContent == null || context.scriptType == null) return;
            runScript(context.scriptType, context.scriptContent, context.scriptName, context.scriptLocation, bridge.contextDirectory, bridge.appCore, bridge.renderer);
        }

        private async void runScript(string scriptType, string scriptContent, string scriptName, string scriptLocation, string contextDirectory, CelestiaAppCore appCore, CelestiaRenderer renderer)
        {
            if (Array.IndexOf(supportedScriptTypes, scriptType) < 0) return;
            if (scriptLocation != null && Array.IndexOf(supportedScriptLocations, scriptLocation) < 0) return;
            if (contextDirectory == null && scriptLocation == "context") return;
            try
            {
                Windows.Storage.StorageFile scriptFile = null;
                var scriptFileName = scriptName == null ? GuidHelper.CreateNewGuid().ToString() + "." + scriptType : scriptName + "." + scriptType;
                if (scriptLocation == "context")
                {
                    var folder = await Windows.Storage.StorageFolder.GetFolderFromPathAsync(contextDirectory);
                    if (folder != null)
                    {
                        scriptFile = await folder.CreateFileAsync(scriptFileName, Windows.Storage.CreationCollisionOption.ReplaceExisting);
                    }
                }
                else
                {
                    var folder = Windows.Storage.ApplicationData.Current.TemporaryFolder;
                    if (folder != null)
                    {
                        scriptFile = await folder.CreateFileAsync(scriptFileName, Windows.Storage.CreationCollisionOption.ReplaceExisting);
                    }
                }
                if (scriptFile == null) return;
                var stream = await scriptFile.OpenAsync(Windows.Storage.FileAccessMode.ReadWrite);
                using (var outputStream = stream.GetOutputStreamAt(0))
                {
                    using (var dataWriter = new Windows.Storage.Streams.DataWriter(outputStream))
                    {
                        dataWriter.WriteString(scriptContent);
                        await dataWriter.StoreAsync();
                        await outputStream.FlushAsync();
                    }
                }
                stream.Dispose();
                renderer.EnqueueTask(() =>
                {
                    appCore.RunScript(scriptFile.Path);
                });
            }
            catch { }
        }
    }

    class SendACKHandler : TypedJavascriptMessageHandler<SendACKContext>
    {
        public override string operation => "sendACK";

        public override void execute(SendACKContext context, JavascriptBridge bridge)
        {
            if (context.id != null && bridge.ackReceived != null)
            {
                bridge.ackReceived(context.id);
            }
        }
    }

    class DummyHandler : TypedJavascriptMessageHandler<DummyContext>
    {
        public override string operation => operationType;

        private string operationType;

        public DummyHandler(string operationType)
        {
            this.operationType = operationType;
        }

        public override void execute(DummyContext context, JavascriptBridge bridge)
        {
            // Ignored
        }
    }

    class RunDemoHandler : TypedJavascriptMessageHandler<RunDemoContext>
    {
        public override string operation => "runDemo";

        public override void execute(RunDemoContext context, JavascriptBridge bridge)
        {
            bridge.renderer.EnqueueTask(() =>
            {
                bridge.appCore.CharEnter(100);
            });
        }
    }

    public delegate void SendACK(string id);

    public class JavascriptBridge
    {
        public CelestiaAppCore appCore;
        public CelestiaRenderer renderer;
        public CoreDispatcher dispatcher;
        public string contextDirectory;
        public SendACK ackReceived;

        private static int supportedScriptVersion = 4;

        private JavascriptMessageHandler[] handlers = new JavascriptMessageHandler[]
        {
            new ShareURLHandler(),
            new RunScriptHandler(),
            new SendACKHandler(),
            new DummyHandler("openAddonNext"), // version 3
            new RunDemoHandler(), // version 4
        };

        public JavascriptBridge(string contextDirectory, CelestiaAppCore appCore, CelestiaRenderer renderer, CoreDispatcher dispatcher)
        {
            this.contextDirectory = contextDirectory;
            this.appCore = appCore;
            this.renderer = renderer;
            this.dispatcher = dispatcher;
        }

        public void receivedMessage(string message)
        {
            try
            {
                var payload = JsonConvert.DeserializeObject<MessagePayload>(message);
                if (payload.operation == null || payload.content == null) return;
                if (payload.minScriptVersion > supportedScriptVersion) return;

                foreach (var handler in handlers)
                {
                    if (handler.operation == payload.operation)
                    {
                        handler.executeWithContent(payload.content, this);
                        return;
                    }
                }
            }
            catch { }
        }
    }
}
