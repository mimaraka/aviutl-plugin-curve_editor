#include "host_object_preset.hpp"

#include <strconv2.h>

#include "preset_manager.hpp"


namespace curve_editor {
	std::wstring PresetHostObject::get_presets_as_json() {
		auto data = nlohmann::json::array();
		for (const auto& preset_info : global::preset_manager.get_preset_info()) {
			nlohmann::json date;
			if (preset_info.date.has_value()) {
				date = preset_info.date.value();
			}
			else {
				date = nullptr;
			}
			data.push_back({
				{ "id", preset_info.id },
				{ "collectionId", preset_info.collection_id },
				{ "name", ::wide_to_utf8(preset_info.name) },
				{ "date", date }
			});
		}
		return ::utf8_to_wide(data.dump());
	}

	std::wstring PresetHostObject::get_collections_as_json() {
		auto data = nlohmann::json::array();
		for (const auto& [id, name] : global::preset_manager.get_collection_info()) {
			data.push_back({
				{ "id", id },
				{ "name", ::wide_to_utf8(name) }
			});
		}
		return ::utf8_to_wide(data.dump());
	}

	std::wstring PresetHostObject::get_filter_info_as_json() {
		auto filter_info = global::preset_manager.get_filter_info();
		return ::utf8_to_wide(nlohmann::json{
			{ "typeNormal", filter_info.type_normal },
			{ "typeValue", filter_info.type_value },
			{ "typeBezier", filter_info.type_bezier },
			{ "typeElastic", filter_info.type_elastic },
			{ "typeBounce", filter_info.type_bounce },
			{ "typeScript", filter_info.type_script }
			}.dump());
	}
} // namespace curve_editor