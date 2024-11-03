#include "host_object_preset.hpp"
#include "preset_manager.hpp"
#include <strconv2.h>



namespace cved {
	std::string PresetHostObject::get_presets_as_json() {
		auto data = nlohmann::json::array();
		for (const auto& preset_info : global::preset_manager.get_preset_info()) {
			data.push_back({
				{ "id", preset_info.id },
				{ "collectionId", preset_info.collection_id },
				{ "name", ::sjis_to_utf8(preset_info.name) }
			});
		}
		return ::utf8_to_sjis(data.dump());
	}

	std::string PresetHostObject::get_collections_as_json() {
		auto data = nlohmann::json::array();
		for (const auto& [id, name] : global::preset_manager.get_collection_info()) {
			data.push_back({
				{ "id", id },
				{ "name", ::sjis_to_utf8(name) }
			});
		}
		return ::utf8_to_sjis(data.dump());
	}
} // namespace cved