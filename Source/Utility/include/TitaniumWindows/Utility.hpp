/**
 * Titanium.Utility for Windows
 * Author: Gary Mathews
 *
 * Copyright (c) 2015 by Appcelerator, Inc. All Rights Reserved.
 * Licensed under the terms of the Apache Public License.
 * Please see the LICENSE included with this distribution for details.
 */

#ifndef _TITANIUMWINDOWS_UTILITY_HPP_
#define _TITANIUMWINDOWS_UTILITY_HPP_

#include <string>
#include <locale>
#include <codecvt>
#include <ppltasks.h>
#include <unordered_map>
#include <chrono>
#include <sstream>
#include <iomanip>
#include <cstdint>

#include "TitaniumWindows_Utility_EXPORT.h"

#define EPOCH_BIAS 116444736000000000 // Number of 100 nanosecond units from 1/1/1601 (windows epoch) to 1/1/1970 (unix epoch)

namespace HAL 
{
	class JSValue;
}

namespace TitaniumWindows
{
	namespace Utility
	{
		//
		// Run a task on the UI thread
		//
		template<class T> static void RunOnUIThread(T handler) {
			RunOnUIThread<T>(Windows::UI::Core::CoreDispatcherPriority::Normal, handler);
		}

		//
		// Run a task on the UI thread with a specified priority
		//
		template<class T> static void RunOnUIThread(Windows::UI::Core::CoreDispatcherPriority priority, T handler) {
			auto dispatcher = Windows::ApplicationModel::Core::CoreApplication::MainView->CoreWindow->Dispatcher;
			dispatcher->RunAsync(priority, ref new Windows::UI::Core::DispatchedHandler(handler));
		}

		//
		// Convert std::string into Platform::String^
		//
		TITANIUMWINDOWS_UTILITY_EXPORT ::Platform::String^ ConvertString(const std::string& str);

		//
		// Convert Platform::String^ into std::string
		//
		TITANIUMWINDOWS_UTILITY_EXPORT std::string ConvertString(::Platform::String^ str);

		//
		// Convert UTF-8 std::string into Platform::String^
		//
		TITANIUMWINDOWS_UTILITY_EXPORT ::Platform::String^ ConvertUTF8String(const std::string& str);

		//
		// Convert Platform::String^ into UTF-8 std::string
		//
		TITANIUMWINDOWS_UTILITY_EXPORT std::string ConvertUTF8String(::Platform::String^ str);

		//
		// Convert unsigned char array into plain-text hex std::string
		//
		TITANIUMWINDOWS_UTILITY_EXPORT std::string HexString(unsigned char* data, size_t length);

		//
		// Get binary content from IBuffer
		//
		TITANIUMWINDOWS_UTILITY_EXPORT std::vector<std::uint8_t> GetContentFromBuffer(Windows::Storage::Streams::IBuffer^ buffer);

		//
		// Get binary content from StorageFile
		//
		TITANIUMWINDOWS_UTILITY_EXPORT std::vector<std::uint8_t> GetContentFromFile(Windows::Storage::StorageFile^ file);

		//
		// Get MIME string from path
		//
		TITANIUMWINDOWS_UTILITY_EXPORT const std::string MimeTypeForExtension(std::string& path);

		//
		// Get milliseconds since the epoch from DateTime
		//
		TITANIUMWINDOWS_UTILITY_EXPORT std::chrono::milliseconds GetMSecSinceEpoch(Windows::Foundation::DateTime d);

		//
		// Get DateTime from JavaScript Date object
		//
		TITANIUMWINDOWS_UTILITY_EXPORT Windows::Foundation::DateTime GetDateTime(const HAL::JSValue& dateObject);

	}  // namespace Utility
}  // namespace TitaniumWindows

#endif  // _TITANIUMWINDOWS_UTILITY_HPP_
