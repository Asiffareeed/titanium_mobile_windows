/**
* Titanium.UI.Label for Windows
*
* Copyright (c) 2014 by Appcelerator, Inc. All Rights Reserved.
* Licensed under the terms of the Apache Public License.
* Please see the LICENSE included with this distribution for details.
*/

#ifndef _TITANIUMWINDOWS_UI_LABEL_HPP_
#define _TITANIUMWINDOWS_UI_LABEL_HPP_

#include "TitaniumWindows_UI_EXPORT.h"
#include "Titanium/UI/Label.hpp"

namespace TitaniumWindows
{
	namespace UI
	{
		using namespace HAL;

		/*!
		  @class

		  @discussion This is the Titanium.UI.Label implementation for
		  Windows.
		*/
		class TITANIUMWINDOWS_UI_EXPORT Label final : public Titanium::UI::Label, public JSExport<Label>
		{
		public:
			Label(const JSContext&) TITANIUM_NOEXCEPT;

			virtual ~Label() = default;
			Label(const Label&) = default;
			Label& operator=(const Label&) = default;
#ifdef TITANIUM_MOVE_CTOR_AND_ASSIGN_DEFAULT_ENABLE
			Label(Label&&) = default;
			Label& operator=(Label&&) = default;
#endif

			static void JSExportInitialize();
			virtual void postCallAsConstructor(const JSContext& js_context, const std::vector<JSValue>& arguments) override;

			virtual void set_color(const std::string& color) TITANIUM_NOEXCEPT override final;
			virtual void set_text(const std::string& text) TITANIUM_NOEXCEPT override final;
			virtual void set_textAlign(const Titanium::UI::TEXT_ALIGNMENT& textAlign) TITANIUM_NOEXCEPT override final;
			virtual void set_verticalAlign(const Titanium::UI::TEXT_VERTICAL_ALIGNMENT& verticalAlign) TITANIUM_NOEXCEPT override final;
			virtual void set_wordWrap(const bool& wordWrap) TITANIUM_NOEXCEPT override final;
			virtual void set_font(const Titanium::UI::Font& font) TITANIUM_NOEXCEPT override final;
			virtual void enableEvent(const std::string& event_name) TITANIUM_NOEXCEPT override final;

			static const std::uint32_t DefaultFontSize = 20;

		private:
#pragma warning(push)
#pragma warning(disable : 4251)
			std::unordered_map<std::string, std::string> custom_fonts__;
#pragma warning(pop)
			Windows::UI::Xaml::Controls::TextBlock^ label__;

			// Event handlers
			Windows::Foundation::EventRegistrationToken click_event_;
			Windows::Foundation::EventRegistrationToken touch_start_event_;
			Windows::Foundation::EventRegistrationToken touch_end_event_;
			Windows::Foundation::EventRegistrationToken touch_move_event_;
			Windows::Foundation::EventRegistrationToken touch_cancel_event_;
		};
	} // namespace UI
} // namespace TitaniumWindows

#endif  // _TITANIUMWINDOWS_UI_LABEL_HPP_
