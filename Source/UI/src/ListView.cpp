/**
 * Titanium.UI.ListView for Windows
 *
 * Copyright (c) 2015 by Appcelerator, Inc. All Rights Reserved.
 * Licensed under the terms of the Apache Public License.
 * Please see the LICENSE included with this distribution for details.
 */

#include "TitaniumWindows/UI/ListView.hpp"
#include "Titanium/UI/ListSection.hpp"
#include "TitaniumWindows/UI/View.hpp"
#include "TitaniumWindows/UI/WindowsViewLayoutDelegate.hpp"
#include "TitaniumWindows/Utility.hpp"
#include "Titanium/detail/TiImpl.hpp"

namespace TitaniumWindows
{
	namespace UI
	{
		using namespace Platform::Collections;
		using namespace Windows::UI::Xaml;

		ListView::ListView(const JSContext& js_context) TITANIUM_NOEXCEPT
			: Titanium::UI::ListView(js_context)
		{
		}

		ListView::~ListView() 
		{
			clearListViewData();
		}

		void ListView::postCallAsConstructor(const JSContext& js_context, const std::vector<JSValue>& arguments)
		{
			Titanium::UI::ListView::postCallAsConstructor(js_context, arguments);	
			
			listview__ = ref new Controls::ListView();
			listview__->IsItemClickEnabled = true;

			resetListViewDataBinding();

			Titanium::UI::ListView::setLayoutDelegate<WindowsViewLayoutDelegate>();
			layoutDelegate__->set_defaultWidth(Titanium::UI::LAYOUT::FILL);
			layoutDelegate__->set_defaultHeight(Titanium::UI::LAYOUT::FILL);

			getViewLayoutDelegate<WindowsViewLayoutDelegate>()->setComponent(listview__);
		}

		void ListView::resetListViewDataBinding() 
		{
			collectionViewSource__ = ref new Data::CollectionViewSource();
			collectionViewItems__  = ref new Vector<Platform::Object^>();
			collectionViewSource__->Source = collectionViewItems__;
			collectionViewSource__->IsSourceGrouped = true;

			auto binding = ref new Data::Binding();
			binding->Source = collectionViewSource__;
			Data::BindingOperations::SetBinding(listview__, Controls::ListView::ItemsSourceProperty, binding);
		}

		void ListView::unregisterSectionLayoutNode(const std::shared_ptr<Titanium::UI::ListSection>& section)
		{
			unregisterListViewItemAsLayoutNode(section->get_headerView());
			unregisterListViewItemAsLayoutNode(section->get_footerView());
			const auto items = section->get_items();
			for (const auto item : items) {
				unregisterListViewItemAsLayoutNode(item.view);
			}
		}
		void ListView::clearListViewData() 
		{
			for (const auto section : model__->get_sections()) {
				unregisterSectionLayoutNode(section);
			}

			resetListViewDataBinding();
			collectionViewItems__->Clear();
		}

		void ListView::JSExportInitialize() 
		{
			JSExport<ListView>::SetClassVersion(1);
			JSExport<ListView>::SetParent(JSExport<Titanium::UI::ListView>::Class());
		}

		void ListView::enableEvent(const std::string& event_name) TITANIUM_NOEXCEPT
		{
			Titanium::UI::ListView::enableEvent(event_name);

			const JSContext ctx = this->get_context();

			if (event_name == "itemclick") {
				itemclick_event__ = listview__->ItemClick += ref new Controls::ItemClickEventHandler(
					[this, ctx](Platform::Object^ sender, Controls::ItemClickEventArgs^ e) {
					auto listview = safe_cast<Controls::ListView^>(sender);

					uint32_t selectedIndex;
					const bool found = listview->Items->IndexOf(e->ClickedItem, &selectedIndex);
					if (!found) return;

					const auto result  = model__->searchRowBySelectedIndex(selectedIndex);
					if (result.found) {
						const auto section = model__->getSectionAtIndex(result.sectionIndex);
						auto this_object = get_object();

						TITANIUM_ASSERT(section->get_items().size() > static_cast<std::uint32_t>(result.rowIndex));
						const auto properties = section->getItemAt(result.rowIndex).properties;
						if (properties.find("itemId") != properties.end()) {
							this_object.SetProperty("_itemclick_itemId_", properties.at("itemId"));
						}
						if (properties.find("bindId") != properties.end()) {
							this_object.SetProperty("_itemclick_bindId_", properties.at("bindId"));
						}
						// we just forward event properties to underlying view so that it can process itemclick event
						this_object.SetProperty("_itemclick_section_", section->get_object());
						this_object.SetProperty("_itemclick_sectionIndex_", ctx.CreateNumber(result.sectionIndex));
						this_object.SetProperty("_itemclick_itemIndex_", ctx.CreateNumber(result.rowIndex));
					}
					// hack: fallback when no view processes the event
					TitaniumWindows::Utility::RunOnUIThread([this](){
						auto this_object = get_object();
						// this means no one catches the event
						if (this_object.HasProperty("_itemclick_section_")) {
							auto eventArgs = get_context().CreateObject();
							
							eventArgs.SetProperty("section", this_object.GetProperty("_itemclick_section_"));
							eventArgs.SetProperty("sectionIndex", this_object.GetProperty("_itemclick_sectionIndex_"));
							eventArgs.SetProperty("itemIndex", this_object.GetProperty("_itemclick_itemIndex_"));
							eventArgs.SetProperty("itemId", this_object.GetProperty("_itemclick_itemId_"));
							eventArgs.SetProperty("bindId", this_object.GetProperty("_itemclick_bindId_"));

							fireEvent("itemclick", eventArgs);

							this_object.DeleteProperty("_itemclick_section_");
							this_object.DeleteProperty("_itemclick_sectionIndex_");
							this_object.DeleteProperty("_itemclick_itemIndex_");
							this_object.DeleteProperty("_itemclick_itemId_");
							this_object.DeleteProperty("_itemclick_bindId_");
						}
					});
				});
			}
		}

		void ListView::disableEvent(const std::string& event_name) TITANIUM_NOEXCEPT
		{
			Titanium::UI::ListView::disableEvent(event_name);

			if (event_name == "itemclick") {
				listview__->ItemClick -= itemclick_event__;
			}
		}

		void ListView::set_searchText(const std::string& query) TITANIUM_NOEXCEPT 
		{
			if (!model__->isSaved()) {
				model__->save();
			}

			querySubmitted(query);
		}

		void ListView::bindCollectionViewSource() 
		{
			collectionViewSource__->Source = collectionViewItems__;
		}

		void ListView::unbindCollectionViewSource()
		{
			collectionViewSource__->Source = ref new Vector<Platform::Object^>();
		}

		void ListView::appendSection(const std::vector<std::shared_ptr<Titanium::UI::ListSection>>& sections, const std::shared_ptr<Titanium::UI::ListViewAnimationProperties>& animation) TITANIUM_NOEXCEPT
		{
			auto start = model__->get_sectionCount();

			Titanium::UI::ListView::appendSection(sections, animation);

			// unbind collection view source to avoid exception
			unbindCollectionViewSource();

			for (std::uint32_t i = 0; i < sections.size(); i++) {
				const auto section = sections.at(i);
				const auto sectionIndex = start + i;
				collectionViewItems__->Append(createUIElementsForSection(sectionIndex));
			}

			// bind collection view again
			bindCollectionViewSource();
		}

		void ListView::deleteSectionAt(const uint32_t& sectionIndex, const std::shared_ptr<Titanium::UI::ListViewAnimationProperties>& animation) TITANIUM_NOEXCEPT
		{
			Titanium::UI::ListView::deleteSectionAt(sectionIndex, animation);

			// unbind collection view source to avoid exception
			unbindCollectionViewSource();

			collectionViewItems__->RemoveAt(sectionIndex);

			// bind collection view again
			bindCollectionViewSource();
		}

		void ListView::insertSectionAt(const uint32_t& sectionIndex, const std::vector<std::shared_ptr<Titanium::UI::ListSection>>& section, const std::shared_ptr<Titanium::UI::ListViewAnimationProperties>& animation) TITANIUM_NOEXCEPT
		{
			Titanium::UI::ListView::insertSectionAt(sectionIndex, section, animation);

			// NOTE: reset all sections beucase collectionViewItems__->InsertAt() messed up views.
			// something to do with section.views JavaScript array maybe? Need to revisit.
			set_sections(model__->get_sections());
		}

		void ListView::replaceSectionAt(const uint32_t& sectionIndex, const std::vector<std::shared_ptr<Titanium::UI::ListSection>>& section, const std::shared_ptr<Titanium::UI::ListViewAnimationProperties>& animation) TITANIUM_NOEXCEPT
		{
			Titanium::UI::ListView::replaceSectionAt(sectionIndex, section, animation);

			// NOTE: reset all sections beucase collectionViewItems__->InsertAt() messed up views. need to revisit.
			// something to do with section.views JavaScript array maybe? Need to revisit.
			set_sections(model__->get_sections());
		}

		void ListView::fireListSectionEvent(const std::string& name, const std::shared_ptr<Titanium::UI::ListSection>& section, const std::uint32_t& itemIndex, const std::uint32_t& itemCount, const std::uint32_t& affectedRows)
		{
			const auto sectionIndex = model__->getSectionIndex(section);
			if (sectionIndex < 0 || collectionViewItems__->Size <= static_cast<std::uint32_t>(sectionIndex)) {
				return;
			}
			// this is list of view for the section including header view
			const auto views = static_cast<Vector<UIElement^>^>(collectionViewItems__->GetAt(sectionIndex));
			
			TITANIUM_ASSERT(views);

			if (name == "append") {
				for (std::uint32_t i = itemIndex; i < itemIndex + itemCount; i++) {
					const auto view = createSectionItemViewAt<TitaniumWindows::UI::View>(sectionIndex, i);
					appendListViewItemForSection(view, views);
					section->setViewForSectionItem(i, view);
				}
			} else if (name == "update" || name == "replace") {
				// "update" and "replace" are basically same, it removes existing content and insert new one
				std::uint32_t index = itemIndex + 1; // +1 because index=0 is header
				for (std::uint32_t i = index; i < index + affectedRows; i++) {
					views->RemoveAt(index);
					unregisterListViewItemAsLayoutNode(section->getViewForSectionItem(i - 1));
					section->setViewForSectionItem(i - 1, nullptr);
				}
				for (std::uint32_t i = itemIndex; i < itemIndex + itemCount; i++) {
					const auto view = createSectionItemViewAt<TitaniumWindows::UI::View>(sectionIndex, i);
					insertListViewItemForSection(view, views, index++);
					section->setViewForSectionItem(i, view);
				}
			} else if (name == "delete") {
				const std::uint32_t index = itemIndex + 1; // +1 because index=0 is header
				TITANIUM_ASSERT(views->Size > index);
				for (std::uint32_t i = index; i < index + itemCount; i++) {
					views->RemoveAt(index);
					unregisterListViewItemAsLayoutNode(section->getViewForSectionItem(i - 1));
					section->setViewForSectionItem(i - 1, nullptr);
				}
			} else if (name == "clear") {
				for (std::uint32_t i = itemIndex; i < itemIndex + itemCount; i++) {
					unregisterListViewItemAsLayoutNode(section->getViewForSectionItem(i));
					section->setViewForSectionItem(i, nullptr);
				}
				if (views->Size > 0) {
					// clear section view except header view
					const auto header = views->GetAt(0);
					views->Clear();
					views->Append(header);
				} else {
					views->Clear();
				}
			}
			Titanium::UI::ListView::fireListSectionEvent(name, section, itemIndex, itemCount, affectedRows);
		}

		Vector<UIElement^>^ ListView::createUIElementsForSection(const std::uint32_t& sectionIndex) TITANIUM_NOEXCEPT
		{
			auto group = ref new Vector<UIElement^>();

			const auto section = model__->getSectionAtIndex(sectionIndex);
			const auto itemsCountToBeCreated = section->get_items().size();
			const auto views = createSectionViewAt<TitaniumWindows::UI::View>(sectionIndex);
			TITANIUM_ASSERT(views.size() == itemsCountToBeCreated);

			// set section header
			const auto headerView = section->get_headerView();
			if (headerView != nullptr) {
				const auto view_delegate = headerView->getViewLayoutDelegate<WindowsViewLayoutDelegate>();
				if (view_delegate == nullptr) {
					TITANIUM_LOG_ERROR("ListView: headerView must be of type Titanium.UI.View");
				} else {
					const auto component = view_delegate->getComponent();
					group->Append(component);

					// add as child view to make layout engine work
					registerListViewItemAsLayoutNode(headerView);
				}
			} else if (section->hasHeaderTitle()) {
				Controls::ListViewHeaderItem^ header = ref new Controls::ListViewHeaderItem();
				const auto headerText = ref new Controls::TextBlock();
				headerText->Text = Utility::ConvertUTF8String(section->get_headerTitle());
				headerText->FontSize = 28; // change this?
				header->Content = headerText;
				group->Append(header);
			}

			for (uint32_t itemIndex = 0; itemIndex < views.size(); itemIndex++) {
				const auto view = views.at(itemIndex);
				appendListViewItemForSection(view, group);
				section->setViewForSectionItem(itemIndex, view);
			}

			// set footer
			const auto footerView = section->get_footerView();
			if (footerView != nullptr) {
				const auto view = dynamic_cast<TitaniumWindows::UI::View*>(footerView.get());
				if (view == nullptr) {
					TITANIUM_LOG_ERROR("ListView: footerView must be of type Titanium.UI.View");
				} else {
					const auto component = view->getComponent();
					group->Append(component);

					// add as child view to make layout engine work
					registerListViewItemAsLayoutNode(footerView);
				}
			} else if (section->hasFooterTitle()) {
				Controls::ListViewHeaderItem^ footer = ref new Controls::ListViewHeaderItem();
				const auto footerText = ref new Controls::TextBlock();
				footerText->Text = Utility::ConvertUTF8String(section->get_footerTitle());
				footerText->FontSize = 28; // change this?
				footer->Content = footerText;
				group->Append(footer);
			}

			return group;
		}

		void ListView::registerListViewItemAsLayoutNode(const std::shared_ptr<Titanium::UI::View>& view)
		{
			if (view == nullptr) {
				return;
			}
			auto layoutDelegate = getViewLayoutDelegate<WindowsViewLayoutDelegate>();
			Titanium::LayoutEngine::nodeAddChild(layoutDelegate->getLayoutNode(), view->getViewLayoutDelegate<WindowsViewLayoutDelegate>()->getLayoutNode());
		}

		void ListView::unregisterListViewItemAsLayoutNode(const std::shared_ptr<Titanium::UI::View>& view) 
		{
			if (view == nullptr) {
				return;
			}
			const auto layout = getViewLayoutDelegate<WindowsViewLayoutDelegate>();
			const auto view_layout = view->getViewLayoutDelegate<WindowsViewLayoutDelegate>();
			Titanium::LayoutEngine::nodeRemoveChild(layout->getLayoutNode(), view_layout->getLayoutNode());
		}

		void ListView::appendListViewItemForSection(const std::shared_ptr<TitaniumWindows::UI::View>& view, Vector<UIElement^>^ group)
		{
			group->Append(view->getComponent());
			registerListViewItemAsLayoutNode(view);
		}

		void ListView::insertListViewItemForSection(const std::shared_ptr<TitaniumWindows::UI::View>& view, Vector<UIElement^>^ group, const std::uint32_t& index) 
		{
			group->InsertAt(index, view->getComponent());
			registerListViewItemAsLayoutNode(view);
		}

		void ListView::set_sections(const std::vector<std::shared_ptr <Titanium::UI::ListSection>>& sections) TITANIUM_NOEXCEPT
		{
			Titanium::UI::ListView::set_sections(sections);

			clearListViewData();

			for (uint32_t sectionIndex = 0; sectionIndex < sections.size(); sectionIndex++) {
				collectionViewItems__->Append(createUIElementsForSection(sectionIndex));
			}
		}
	}  // namespace UI
}  // namespace TitaniumWindows