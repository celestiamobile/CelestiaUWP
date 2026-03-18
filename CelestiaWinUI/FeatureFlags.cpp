// FeatureFlags.cpp
//
// Copyright (C) 2025, Celestia Development Team
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.

#include "pch.h"
#include "FeatureFlags.h"
#if __has_include("FeatureFlags.g.cpp")
#include "FeatureFlags.g.cpp"
#endif

#include <random>

using namespace std;
using namespace winrt;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::Data::Json;
using namespace Windows::Web::Http;

namespace winrt::CelestiaWinUI::implementation
{
    // Add new flag keys here
    static const vector<hstring> flagKeys = { L"dummy" };

    FeatureFlags::FeatureFlags() {}

    FeatureFlags::FeatureFlags(unordered_map<hstring, bool> const& flags) : flags(flags)
    {
    }

    bool FeatureFlags::Lookup(hstring const& key)
    {
        auto it = flags.find(key);
        return it != flags.end() ? it->second : false;
    }

    bool FeatureFlags::Dummy() { return Lookup(L"dummy"); }

    CelestiaWinUI::FeatureFlags FeatureFlags::Get(Windows::Storage::ApplicationDataContainer const& settings)
    {
        unordered_map<hstring, bool> evaluated;
        auto stored = settings.Values().TryLookup(L"FeatureFlags");
        if (stored)
        {
            try
            {
                auto json = JsonObject::Parse(unbox_value<hstring>(stored));
                for (auto const& key : flagKeys)
                {
                    auto jsonValue = json.TryLookup(key);
                    if (jsonValue && jsonValue.ValueType() == JsonValueType::Boolean)
                        evaluated[key] = jsonValue.GetBoolean();
                }
            }
            catch (hresult_error const&) {}
        }
        return make<FeatureFlags>(evaluated);
    }

    IAsyncAction FeatureFlags::UpdateAsync(bool isXbox, Windows::Storage::ApplicationDataContainer const& settings)
    {
        HttpClient client;
        HttpFormUrlEncodedContent query({ {L"platform", isXbox ? L"xbox" : L"uwp"}, {L"lang", CelestiaAppComponent::LocalizationHelper::Locale()} });
        auto url = hstring(L"https://celestia.mobi/api/2") + L"/resource/features" + L"?" + query.ToString();
        auto content = co_await client.GetStringAsync(Uri(url));

        // Load or generate a stable device ID
        hstring deviceId;
        auto storedDeviceId = settings.Values().TryLookup(L"FeatureFlagsDeviceID");
        if (storedDeviceId)
        {
            deviceId = unbox_value<hstring>(storedDeviceId);
        }
        else
        {
            mt19937_64 rng(random_device{}());
            uniform_int_distribution<uint64_t> dist;
            deviceId = to_hstring(dist(rng));
            settings.Values().Insert(L"FeatureFlagsDeviceID", box_value(deviceId));
        }

        auto object = JsonObject::Parse(content);
        unordered_map<hstring, bool> evaluated;
        for (auto const& key : flagKeys)
        {
            auto jsonValue = object.TryLookup(key);
            if (jsonValue && jsonValue.ValueType() == JsonValueType::Number)
            {
                auto progress = jsonValue.GetNumber();
                auto combined = to_string(deviceId) + to_string(key);
                double seed = static_cast<double>(hash<string>{}(combined)) / static_cast<double>(SIZE_MAX);
                evaluated[key] = seed < progress;
            }
        }

        JsonObject json;
        for (auto const& key : flagKeys)
        {
            auto it = evaluated.find(key);
            json.SetNamedValue(key, JsonValue::CreateBooleanValue(it != evaluated.end() && it->second));
        }

        settings.Values().Insert(L"FeatureFlags", box_value(json.Stringify()));
    }
}
