/**
 * Titanium.UI.TableViewRow for Windows
 *
 * Copyright (c) 2015 by Appcelerator, Inc. All Rights Reserved.
 * Licensed under the terms of the Apache Public License.
 * Please see the LICENSE included with this distribution for details.
 */

#ifndef _TITANIUMWINDOWS_TABLEVIEWROW_HPP_
#define _TITANIUMWINDOWS_TABLEVIEWROW_HPP_

#include "TitaniumWindows_UI_EXPORT.h"
#include "Titanium/UI/TableViewRow.hpp"

namespace TitaniumWindows
{
	namespace UI
	{
		using namespace HAL;

		class Label;

		/*!
		  @class

		  @discussion This is the Titanium.UI.TableViewRow implementation for Windows.
		*/
#pragma warning(push)
#pragma warning(disable : 4275)
		class TITANIUMWINDOWS_UI_EXPORT TableViewRow final : public Titanium::UI::TableViewRow, public JSExport<TableViewRow>
		{
#pragma warning(pop)
		public:
			TableViewRow(const JSContext&) TITANIUM_NOEXCEPT;
			virtual ~TableViewRow()                  = default;
			TableViewRow(const TableViewRow&)            = default;
			TableViewRow& operator=(const TableViewRow&) = default;
#ifdef TITANIUM_MOVE_CTOR_AND_ASSIGN_DEFAULT_ENABLE
			TableViewRow(TableViewRow&&)                 = default;
			TableViewRow& operator=(TableViewRow&&)      = default;
#endif

			static void JSExportInitialize();

			virtual void postCallAsConstructor(const JSContext& js_context, const std::vector<JSValue>& arguments) override;

			virtual void add(const JSObject& view, JSObject& this_object) TITANIUM_NOEXCEPT;
			virtual void set_title(const std::string& title) TITANIUM_NOEXCEPT override;

		private:
#pragma warning(push)
#pragma warning(disable : 4251)
			Windows::UI::Xaml::Controls::Canvas^ content__ = {nullptr};
			std::shared_ptr<Label> title__ = {nullptr};
#pragma warning(pop)
		};
	}  // namespace UI
}  // namespace TitaniumWindows
#endif // _TITANIUMWINDOWS_TABLEVIEWROW_HPP_