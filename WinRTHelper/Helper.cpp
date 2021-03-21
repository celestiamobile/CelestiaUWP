#include "pch.h"
#include "Helper.h"
#if __has_include("Helper.g.cpp")
#include "Helper.g.cpp"
#endif

using namespace std;

namespace winrt::WinRTHelper::implementation
{
	void Helper::PropertySetAddSize(Windows::Foundation::Collections::PropertySet const& propertySet, hstring const& key, Windows::Foundation::Size const& value)
	{
		propertySet.Insert(key, Windows::Foundation::PropertyValue::CreateSize(value));
	}

	void Helper::PropertySetAddSingle(Windows::Foundation::Collections::PropertySet const& propertySet, hstring const& key, float value)
	{
		propertySet.Insert(key, Windows::Foundation::PropertyValue::CreateSingle(value));
	}
}
