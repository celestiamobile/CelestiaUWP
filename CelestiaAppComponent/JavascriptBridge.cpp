#include "pch.h"
#include "JavascriptBridge.h"
#if __has_include("JavascriptMessagePayload.g.cpp")
#include "JavascriptMessagePayload.g.cpp"
#endif
#if __has_include("JavascriptBridge.g.cpp")
#include "JavascriptBridge.g.cpp"
#endif

using namespace std;
using namespace winrt;
using namespace Windows::Foundation;
using namespace Windows::Data::Json;

namespace winrt::CelestiaAppComponent::implementation
{
    JavascriptMessagePayload::JavascriptMessagePayload(hstring const& operation, hstring const& content, int32_t minScriptVersion)
        : operation(operation), content(content), minScriptVersion(minScriptVersion)
    {
    }

    hstring JavascriptMessagePayload::Operation()
    {
        return operation;
    }

    hstring JavascriptMessagePayload::Content()
    {
        return content;
    }

    int32_t JavascriptMessagePayload::MinScriptVersion()
    {
        return minScriptVersion;
    }

    int32_t supportedScriptVersion = 4;

    CelestiaAppComponent::JavascriptMessagePayload JavascriptMessagePayload::TryParse(hstring const& message)
    {
        JsonObject object;
        if (JsonObject::TryParse(message, object))
        {
            try
            {
                auto operation = object.GetNamedString(L"operation");
                auto content = object.GetNamedString(L"content");
                auto minScriptVersion = (int32_t)object.GetNamedNumber(L"minScriptVersion");
                return make<JavascriptMessagePayload>(operation, content, minScriptVersion);
            }
            catch(hresult_error const&) {}
        }
        return nullptr;
    }

    RunDemoHandler::RunDemoHandler() {};

    hstring RunDemoHandler::Operation()
    {
        return L"runDemo";
    }

    void RunDemoHandler::ExecuteWithContent(hstring const&, CelestiaAppComponent::JavascriptMessageHandlerContext const& context)
    {
        auto appCore = context.AppCore();
        context.Renderer().EnqueueTask([appCore]()
            {
                appCore.CharEnter(100);
            });
    }

    RunScriptHandler::RunScriptHandler() {};

    hstring RunScriptHandler::Operation()
    {
        return L"runScript";
    }

    fire_and_forget RunScriptHandler_ExecuteWithContent(hstring const& content, CelestiaAppComponent::JavascriptMessageHandlerContext const& context)
    {
        const std::vector<hstring> supportedScriptLocation { L"temp", L"context" };
        const std::vector<hstring> supportedScriptTypes = { L"cel", L"celx" };
        auto appCore = context.AppCore();
        auto renderer = context.Renderer();
        auto contextDirectory = context.ContextDirectory();

        JsonObject object;
        if (JsonObject::TryParse(content, object))
        {
            auto scriptContent = object.GetNamedString(L"scriptContent", L"");
            auto scriptLocation = object.GetNamedString(L"scriptLocation", L"");
            auto scriptType = object.GetNamedString(L"scriptType", L"");
            auto scriptName = object.GetNamedString(L"scriptName", L"");
            if (scriptContent.empty()) return;
            if (std::find(supportedScriptTypes.begin(), supportedScriptTypes.end(), scriptType) == supportedScriptTypes.end()) return;
            if (!scriptLocation.empty() && std::find(supportedScriptLocation.begin(), supportedScriptLocation.end(), scriptLocation) == supportedScriptLocation.end()) return;
            if (contextDirectory.empty() && scriptLocation == L"context") return;

            using namespace Windows::Storage;
            try
            {
                StorageFile file{ nullptr };
                auto scriptFileName = scriptName.empty() ? to_hstring(GuidHelper::CreateNewGuid()) + L"." + scriptType : scriptName + L"." + scriptType;
                if (scriptLocation == L"context")
                {
                    auto folder = co_await StorageFolder::GetFolderFromPathAsync(contextDirectory);
                    if (folder != nullptr)
                        file = co_await folder.CreateFileAsync(scriptFileName, CreationCollisionOption::ReplaceExisting);
                }
                else
                {
                    auto folder = ApplicationData::Current().TemporaryFolder();
                    if (folder != nullptr)
                        file = co_await folder.CreateFileAsync(scriptFileName, CreationCollisionOption::ReplaceExisting);
                }
                if (file == nullptr) co_return;
                FileIO::WriteTextAsync(file, scriptContent);
                renderer.EnqueueTask([appCore, file]()
                    {
                        appCore.RunScript(file.Path());
                    });
            }
            catch (hresult_error const&) {}
        }
    }

    void RunScriptHandler::ExecuteWithContent(hstring const& content, CelestiaAppComponent::JavascriptMessageHandlerContext const& context)
    {
        RunScriptHandler_ExecuteWithContent(content, context);
    }

    ShareURLHandler::ShareURLHandler() {};

    hstring ShareURLHandler::Operation()
    {
        return L"shareURL";
    }

    void ShareURLHandler::ExecuteWithContent(hstring const& content, CelestiaAppComponent::JavascriptMessageHandlerContext const& context)
    {
        JsonObject object;
        if (JsonObject::TryParse(content, object))
        {
            auto title = object.GetNamedString(L"title", L"");
            auto url = object.GetNamedString(L"url", L"");
            if (!title.empty() && !url.empty())
            {
                auto callback = context.ShareCallback();
                if (callback != nullptr)
                    callback(title, Uri(url));
            }
        }
    }

    SendACKHandler::SendACKHandler() {};

    hstring SendACKHandler::Operation()
    {
        return L"sendACK";
    }

    void SendACKHandler::ExecuteWithContent(hstring const& content, CelestiaAppComponent::JavascriptMessageHandlerContext const& context)
    {
        JsonObject object;
        if (JsonObject::TryParse(content, object))
        {
            auto id = object.GetNamedString(L"id", L"");
            if (!id.empty() && context.ACKCallback() != nullptr)
            {
                context.ACKCallback()(id);
            }
        }
    }

    JavascriptBridge::JavascriptBridge(const CelestiaComponent::CelestiaAppCore& appCore, const CelestiaComponent::CelestiaRenderer& renderer, const hstring& contextDirectory, CelestiaAppComponent::JavascriptMessageACKCallback const& ackCallback, CelestiaAppComponent::JavascriptMessageShareCallback const& shareCallback)
        : appCore(appCore), renderer(renderer), contextDirectory(contextDirectory), ackCallback(ackCallback), shareCallback(shareCallback)
    {
        handlers = single_threaded_vector<CelestiaAppComponent::JavascriptMessageHandler>(std::vector<CelestiaAppComponent::JavascriptMessageHandler>(
            {
                make<ShareURLHandler>(),
                make<RunScriptHandler>(),
                make<SendACKHandler>(),
                make<DummyHandler>(L"openAddonNext"), // version 3
                make<RunDemoHandler>(), // version 4
            }));
    }

    void JavascriptBridge::ReceivedMessage(hstring const& message)
    {
        auto parsed = JavascriptMessagePayload::TryParse(message);
        if (parsed != nullptr && parsed.MinScriptVersion() <= supportedScriptVersion)
        {
            for (const auto& handler : handlers)
            {
                if (handler.Operation() == parsed.Operation())
                {
                    handler.ExecuteWithContent(parsed.Content(), make<JavascriptMessageHandlerContext>(appCore, renderer, contextDirectory, ackCallback, shareCallback));
                    return;
                }
            }
        }
    }
}
