#include "pch.h"
#include "EmailHelper.h"
#if __has_include("EmailHelper.g.cpp")
#include "EmailHelper.g.cpp"
#endif

using namespace winrt;
using namespace Windows::Foundation;

namespace winrt::CelestiaWinUI::implementation
{
    hstring AddField(hstring const& uri, hstring const& key, hstring const& value, bool& isFirst)
    {
        if (key.empty() || value.empty())
            return uri;
        hstring result = uri;
        if (isFirst)
        {
            isFirst = false;
            result = result + L"?";
        }
        else
        {
            result = result + L"&";
        }
        return result + key + L"=" + value;
    }

    Uri EmailHelper::UriForEmail(hstring const& recipient, hstring const& subject, hstring const& body, Collections::IVector<Windows::Storage::StorageFile> const& attachments)
    {
        hstring uri = L"mailto:";
        if (!recipient.empty())
            uri = uri + Uri::EscapeComponent(recipient);
        bool isFirstComponent = true;
        if (attachments != nullptr && attachments.Size() > 0)
        {
            hstring attachmentString = L"";
            for (auto const& attachment : attachments)
            {
                hstring token = L"";
                try
                {
                    token = Windows::ApplicationModel::DataTransfer::SharedStorageAccessManager::AddFile(attachment);
                }
                catch (hresult_error const&)
                {}
                if (token.empty())
                    return nullptr;

                if (!attachmentString.empty())
                    attachmentString = attachmentString + L",";
                attachmentString = attachmentString + Uri::EscapeComponent(token);
            }
            uri = AddField(uri, L"ms-attachments", attachmentString, isFirstComponent);
        }

        uri = AddField(uri, L"subject", subject, isFirstComponent);
        uri = AddField(uri, L"body", body, isFirstComponent);
        return Uri{ uri };
    }
}
