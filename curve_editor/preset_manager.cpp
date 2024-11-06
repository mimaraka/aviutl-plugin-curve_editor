#include "config.hpp"
#include "curve_bezier.hpp"
#include "curve_elastic.hpp"
#include "curve_bounce.hpp"
#include "preset_manager.hpp"
#include "string_table.hpp"
#include <fstream>
#include <strconv2.h>



namespace cved::global {
	PresetManager::PresetManager() noexcept :
		collection_info_{
			{COLLECTION_ID_ALL, string_table[StringTable::StringId::LabelCollectionNameAll]},
			{COLLECTION_ID_DEFAULT, string_table[StringTable::StringId::LabelCollectionNameDefault]},
			{COLLECTION_ID_ROOT, string_table[StringTable::StringId::LabelCollectionNameRoot]}
	}
	{
		constexpr std::tuple<const char*, EditMode, int32_t> default_curves[] = {
			{"linear", EditMode::Bezier, 345633042},
			{"easeInSine", EditMode::Bezier, -343094698},
			{"easeOutSine", EditMode::Bezier, 344859039},
			{"easeInOutSine", EditMode::Bezier, 343215240},
			{"easeOutInSine", EditMode::Bezier, -343215241},
			{"easeInQuad", EditMode::Bezier, -342375882},
			{"easeOutQuad", EditMode::Bezier, 344924375},
			{"easeInOutQuad", EditMode::Bezier, 342692472},
			{"easeOutInQuad", EditMode::Bezier, -342692473},
			{"easeInCubic", EditMode::Bezier, -341264962},
			{"easeOutCubic", EditMode::Bezier, 343552071},
			{"easeInOutCubic", EditMode::Bezier, 341385552},
			{"easeOutInCubic", EditMode::Bezier, -341385553},
			{"easeInQuart", EditMode::Bezier, -340742168},
			{"easeOutQuart", EditMode::Bezier, 342375817},
			{"easeInOutQuart", EditMode::Bezier, 340666746},
			{"easeOutInQuart", EditMode::Bezier, -340666747},
			{"easeInQuint", EditMode::Bezier, -340546113},
			{"easeOutQuint", EditMode::Bezier, 341460956},
			{"easeInOutQuint", EditMode::Bezier, 340209324},
			{"easeOutInQuint", EditMode::Bezier, -340209325},
			{"easeInExpo", EditMode::Bezier, -340154025},
			{"easeOutExpo", EditMode::Bezier, 341068868},
			{"easeInOutExpo", EditMode::Bezier, 339947940},
			{"easeOutInExpo", EditMode::Bezier, -339947941},
			{"easeInCirc", EditMode::Bezier, -42106373},
			{"easeOutCirc", EditMode::Bezier, 342044512},
			{"easeInOutCirc", EditMode::Bezier, 340078632},
			{"easeOutInCirc", EditMode::Bezier, -340078633},
			{"easeInBack", EditMode::Bezier, -710932937},
			{"easeOutBack", EditMode::Bezier, 343296340},
			{"easeInOutBack", EditMode::Bezier, 737186274},
			{"easeOutInBack", EditMode::Bezier, -737186275},
			{"easeInElastic", EditMode::Elastic, -1544896},
			{"easeOutElastic", EditMode::Elastic, 1544896},
			{"easeInBounce", EditMode::Bounce, -10758499},
			{"easeOutBounce", EditMode::Bounce, 10758499}
		};

		for (const auto& [name, mode, code] : default_curves) {
			Preset preset{ COLLECTION_ID_DEFAULT };

			switch (mode) {
			case EditMode::Bezier:
			{
				BezierCurve curve;
				curve.decode(code);
				preset.create(curve, name);
				break;
			}

			case EditMode::Elastic:
			{
				ElasticCurve curve;
				curve.decode(code);
				preset.create(curve, name);
				break;
			}

			case EditMode::Bounce:
			{
				BounceCurve curve;
				curve.decode(code);
				preset.create(curve, name);
				break;
			}

			default:
				continue;
			}
			presets_.emplace_back(std::move(preset));
		}
	}

	uint32_t PresetManager::create_collection(const std::string& name, bool change_current_id) noexcept {
		auto new_collection_id = collection_id_counter_++;
		collection_info_.emplace_back(new_collection_id, name);
		if (change_current_id) {
			current_collection_id_ = new_collection_id;
		}
		return new_collection_id;
	}

	bool PresetManager::import_collection(const std::filesystem::path& path) noexcept {
		std::ifstream ifs{ path };
		if (!ifs) return false;
		nlohmann::json data;
		try {
			data = nlohmann::json::parse(ifs);
			auto name = data.at("name").get<std::string>();
			create_collection(name);
			auto presets = data.at("presets");
			for (const auto& preset_data : presets) {
				Preset preset{ current_collection_id_ };
				if (!preset.load_json(preset_data)) {
					return false;
				}
				presets_.emplace_back(std::move(preset));
			}
			return true;
		}
		catch (const nlohmann::json::exception&) {
			return false;
		}
	}

	bool PresetManager::export_collection(uint32_t collection_id, const std::filesystem::path& path) const noexcept {
		nlohmann::json data = nlohmann::json::object();
		data["presets"] = nlohmann::json::array();
		std::string name;
		if (collection_id == COLLECTION_ID_ALL) {
			name = path.stem().string();
		}
		else {
			name = get_collection_name(collection_id);
		}
		data["name"] = ::sjis_to_utf8(name);
		for (const auto& preset : presets_) {
			if (collection_id == COLLECTION_ID_ALL or collection_id == preset.get_collection_id()) {
				data["presets"].push_back(preset.create_json());
			}
		}
		std::ofstream ofs{ path };
		if (!ofs) return false;
		ofs << data.dump(2);
		return true;
	}

	bool PresetManager::remove_preset(uint32_t id) noexcept {
		for (auto it = presets_.begin(); it != presets_.end(); it++) {
			if (id == (*it).get_id()) {
				presets_.erase(it);
				return true;
			}
		}
		return false;
	}

	void PresetManager::rename_preset(uint32_t id, const std::string& name) noexcept {
		for (auto& preset : presets_) {
			if (id == preset.get_id()) {
				preset.set_name(name);
				return;
			}
		}
	}

	std::string PresetManager::get_preset_name(uint32_t id) const noexcept {
		for (const auto& preset : presets_) {
			if (id == preset.get_id()) {
				return preset.get_name();
			}
		}
		return "";
	}

	bool PresetManager::is_preset_default(uint32_t id) const noexcept {
		for (const auto& preset : presets_) {
			if (id == preset.get_id()) {
				return preset.get_collection_id() == COLLECTION_ID_DEFAULT;
			}
		}
		return false;
	}

	bool PresetManager::remove_collection(uint32_t collection_id) noexcept {
		if (!is_collection_custom(collection_id)) {
			return false;
		}
		auto iter = std::find_if(collection_info_.begin(), collection_info_.end(), [collection_id](const auto& info) {
			return info.first == collection_id;
			});
		if (iter == collection_info_.end()) {
			return false;
		}
		collection_info_.erase(iter);
		for (auto it = presets_.begin(); it != presets_.end();) {
			if (collection_id == it->get_collection_id()) {
				it = presets_.erase(it);
			}
			else {
				it++;
			}
		}
		if (current_collection_id_ == collection_id) {
			current_collection_id_ = COLLECTION_ID_ALL;
		}
		return true;
	}

	bool PresetManager::rename_collection(uint32_t collection_id, const std::string& name) noexcept {
		if (!is_collection_custom(collection_id)) {
			return false;
		}
		for (auto& [id, collection_name] : collection_info_) {
			if (id == collection_id) {
				collection_name = name;
				return true;
			}
		}
		return false;
	}

	void PresetManager::set_current_collection_id(uint32_t collection_id) noexcept {
		for (const auto& info : collection_info_) {
			if (info.first == collection_id) {
				current_collection_id_ = collection_id;
				return;
			}
		}
	}

	std::string PresetManager::get_collection_name(uint32_t collection_id) const noexcept {
		for (const auto& [id, name] : collection_info_) {
			if (id == collection_id) {
				return name;
			}
		}
		return "";
	}

	bool PresetManager::save_json() const noexcept {
		nlohmann::json data = nlohmann::json::object();
		data["collections"] = nlohmann::json::array();

		data["collections"].push_back({
			{ "type", "root" },
			{ "presets", nlohmann::json::array() }
			});
		std::vector<uint32_t> custom_collection_ids;
		for (const auto& [id, name] : collection_info_) {
			if (is_collection_custom(id)) {
				data["collections"].push_back({
					{ "type", "custom" },
					{ "name", ::sjis_to_utf8(name)},
					{ "presets", nlohmann::json::array() }
					});
				custom_collection_ids.emplace_back(id);
			}
		}

		for (const auto& preset : presets_) {
			auto collection_id = preset.get_collection_id();
			if (collection_id == COLLECTION_ID_DEFAULT) {
				continue;
			}
			else if (collection_id == COLLECTION_ID_ROOT) {
				data["collections"][0]["presets"].push_back(preset.create_json());
				continue;
			}
			auto iter = std::find(custom_collection_ids.begin(), custom_collection_ids.end(), collection_id);
			if (iter == custom_collection_ids.end()) {
				continue;
			}
			auto idx = std::distance(custom_collection_ids.begin(), iter) + 1;
			data["collections"][idx]["presets"].push_back(preset.create_json());
		}

		std::ofstream ofs{ config.get_dir_plugin() / PRESET_FILE_NAME };
		if (!ofs) return false;
		ofs << data.dump();
		return true;
	}

	bool PresetManager::load_json() noexcept {
		std::ifstream ifs{ config.get_dir_plugin() / PRESET_FILE_NAME };
		if (!ifs) return false;
		nlohmann::json data;
		try {
			data = nlohmann::json::parse(ifs);
			auto collections = data.at("collections");
			for (const auto& collection : collections) {
				auto type = collection.at("type").get<std::string>();
				uint32_t collection_id;
				if (type == "root") {
					collection_id = COLLECTION_ID_ROOT;
				}
				else if (type == "custom") {
					auto name = ::utf8_to_sjis(collection.at("name").get<std::string>());
					collection_id = create_collection(name, false);
				}
				else {
					return false;
				}
				auto presets = collection.at("presets");
				for (const auto& preset_data : presets) {
					Preset preset{ collection_id };
					if (!preset.load_json(preset_data)) {
						return false;
					}
					presets_.emplace_back(std::move(preset));
				}
			}
		}
		catch (const nlohmann::json::exception&) {
			return false;
		}
		return true;
	}

	std::vector<PresetManager::PresetInfo> PresetManager::get_preset_info() const noexcept {
		std::vector<PresetInfo> info;
		for (const auto& preset : presets_) {
			info.push_back({
				preset.get_id(),
				preset.get_collection_id(),
				preset.get_name()
				});
		}
		return info;
	}
} // namespace cved::global