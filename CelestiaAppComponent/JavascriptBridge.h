#pragma once

#include "JavascriptMessagePayload.g.h"
#include "JavascriptMessageHandlerContext.g.h"
#include "JavascriptMessageHandler.g.h"
#include "DummyHandler.g.h"
#include "RunDemoHandler.g.h"
#include "RunScriptHandler.g.h"
#include "ShareURLHandler.g.h"
#include "SendACKHandler.g.h"
#include "JavascriptBridge.g.h"

namespace winrt::CelestiaAppComponent::implementation
{
    struct JavascriptMessagePayload : JavascriptMessagePayloadT<JavascriptMessagePayload>
    {
        JavascriptMessagePayload(hstring const& operation, hstring const& content, int32_t minScriptVersion);
        hstring Operation();
        hstring Content();
        int32_t MinScriptVersion();

        static CelestiaAppComponent::JavascriptMessagePayload TryParse(hstring const& message);
    private:
        hstring operation;
        hstring content;
        int32_t minScriptVersion;
    };

    struct JavascriptMessageHandlerContext : JavascriptMessageHandlerContextT<JavascriptMessageHandlerContext>
    {
        JavascriptMessageHandlerContext(CelestiaComponent::CelestiaAppCore const& appCore, CelestiaComponent::CelestiaRenderer const& renderer, hstring const& contextDirectory, CelestiaAppComponent::JavascriptMessageACKCallback const& ackCallback, CelestiaAppComponent::JavascriptMessageShareCallback const& shareCallback) : appCore(appCore), renderer(renderer), contextDirectory(contextDirectory), ackCallback(ackCallback), shareCallback(shareCallback) {};
        CelestiaComponent::CelestiaAppCore AppCore() { return appCore; };
        CelestiaComponent::CelestiaRenderer Renderer() { return renderer; };
        hstring ContextDirectory() { return contextDirectory; };
        CelestiaAppComponent::JavascriptMessageACKCallback ACKCallback() { return ackCallback; };
        CelestiaAppComponent::JavascriptMessageShareCallback ShareCallback() { return shareCallback; };

    private:
        CelestiaComponent::CelestiaAppCore appCore;
        CelestiaComponent::CelestiaRenderer renderer;
        hstring contextDirectory;
        CelestiaAppComponent::JavascriptMessageACKCallback ackCallback;
        CelestiaAppComponent::JavascriptMessageShareCallback shareCallback;
    };

    struct JavascriptMessageHandler : JavascriptMessageHandlerT<JavascriptMessageHandler>
    {
        virtual hstring Operation() = 0;
        virtual void ExecuteWithContent(hstring const& content, CelestiaAppComponent::JavascriptMessageHandlerContext const& context) = 0;
    };

    struct DummyHandler : DummyHandlerT<DummyHandler, JavascriptMessageHandler>
    {
        DummyHandler(hstring const& operation) : operation(operation) {};
        hstring Operation() { return operation; };
        void ExecuteWithContent(hstring const&, CelestiaAppComponent::JavascriptMessageHandlerContext const&) {};
    private:
        hstring operation;
    };

    struct RunDemoHandler : RunDemoHandlerT<RunDemoHandler, JavascriptMessageHandler>
    {
        RunDemoHandler();
        hstring Operation();
        void ExecuteWithContent(hstring const&, CelestiaAppComponent::JavascriptMessageHandlerContext const&);
    };

    struct RunScriptHandler : RunScriptHandlerT<RunScriptHandler, JavascriptMessageHandler>
    {
        RunScriptHandler();
        hstring Operation();
        void ExecuteWithContent(hstring const&, CelestiaAppComponent::JavascriptMessageHandlerContext const&);
    };

    struct ShareURLHandler : ShareURLHandlerT<ShareURLHandler, JavascriptMessageHandler>
    {
        ShareURLHandler();
        hstring Operation();
        void ExecuteWithContent(hstring const&, CelestiaAppComponent::JavascriptMessageHandlerContext const&);
    };

    struct SendACKHandler : SendACKHandlerT<SendACKHandler, JavascriptMessageHandler>
    {
        SendACKHandler();
        hstring Operation();
        void ExecuteWithContent(hstring const&, CelestiaAppComponent::JavascriptMessageHandlerContext const&);
    };

    struct JavascriptBridge : JavascriptBridgeT<JavascriptBridge>
    {
        JavascriptBridge(CelestiaComponent::CelestiaAppCore const& appCore, CelestiaComponent::CelestiaRenderer const& renderer, hstring const& contextDirectory, CelestiaAppComponent::JavascriptMessageACKCallback const& ackCallback, CelestiaAppComponent::JavascriptMessageShareCallback const& shareCallback);

        void ReceivedMessage(hstring const& message);

    private:
        CelestiaComponent::CelestiaAppCore appCore;
        CelestiaComponent::CelestiaRenderer renderer;
        hstring contextDirectory;
        CelestiaAppComponent::JavascriptMessageACKCallback ackCallback;
        CelestiaAppComponent::JavascriptMessageShareCallback shareCallback;
        Windows::Foundation::Collections::IVector<CelestiaAppComponent::JavascriptMessageHandler> handlers;
    };
}

namespace winrt::CelestiaAppComponent::factory_implementation
{
    struct JavascriptMessagePayload : JavascriptMessagePayloadT<JavascriptMessagePayload, implementation::JavascriptMessagePayload>
    {
    };

    struct JavascriptBridge : JavascriptBridgeT<JavascriptBridge, implementation::JavascriptBridge>
    {
    };
}
