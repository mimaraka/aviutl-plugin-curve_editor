#pragma once

#include "curve_editor.hpp"
#include "preset_manager.hpp"
#include <mkaul/host_object.hpp>



namespace cved {
	class PresetHostObject : public mkaul::ole::HostObject {
		static std::string get_presets_as_json();
		static std::string get_collections_as_json();

	public:
		PresetHostObject() {
			register_member(L"getPresetsAsJson", DispatchType::Method, get_presets_as_json);
			register_member(L"getCollectionsAsJson", DispatchType::Method, get_collections_as_json);
			register_member(L"getCollectionName", DispatchType::Method, +[](uint32_t id) { return global::preset_manager.get_collection_name(id); });
			register_member(L"currentCollectionId", DispatchType::PropertyGet, +[]() { return global::preset_manager.get_current_collection_id(); });
			register_member(L"currentCollectionId", DispatchType::PropertyPut, +[](uint32_t id) { global::preset_manager.set_current_collection_id(id); });
		}
	};
} // namespace cved