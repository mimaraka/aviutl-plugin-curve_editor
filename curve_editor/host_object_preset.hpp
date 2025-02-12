#pragma once

#include <magic_enum/magic_enum.hpp>
#include <mkaul/host_object.hpp>

#include "curve_editor.hpp"
#include "preset_manager.hpp"



namespace curve_editor {
	class PresetHostObject : public mkaul::ole::HostObject {
		static std::string get_presets_as_json();
		static std::string get_collections_as_json();
		static std::string get_filter_info_as_json();

	public:
		PresetHostObject() {
			register_member(L"getPresetsAsJson", DispatchType::Method, get_presets_as_json);
			register_member(L"getCollectionsAsJson", DispatchType::Method, get_collections_as_json);
			register_member(L"getCollectionName", DispatchType::Method, +[](uint32_t id) { return global::preset_manager.get_collection_name(id); });
			register_member(L"currentCollectionId", DispatchType::PropertyGet, +[]() { return global::preset_manager.get_current_collection_id(); });
			register_member(L"currentCollectionId", DispatchType::PropertyPut, +[](uint32_t id) { global::preset_manager.set_current_collection_id(id); });
			register_member(L"sortBy", DispatchType::PropertyGet, +[]() { return magic_enum::enum_name(global::preset_manager.get_sort_by()).data(); });
			register_member(L"sortOrder", DispatchType::PropertyGet, +[]() { return magic_enum::enum_name(global::preset_manager.get_sort_order()).data(); });
			register_member(L"getFilterInfoAsJson", DispatchType::Method, get_filter_info_as_json);
		}
	};
} // namespace curve_editor