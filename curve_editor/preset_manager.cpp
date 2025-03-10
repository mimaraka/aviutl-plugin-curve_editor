#include "config.hpp"
#include "curve_bezier.hpp"
#include "curve_elastic.hpp"
#include "curve_bounce.hpp"
#include "preset_manager.hpp"
#include "string_table.hpp"
#include <fstream>
#include <strconv2.h>



namespace curve_editor::global {
	PresetManager::PresetManager() noexcept :
		collection_info_{
			{COLLECTION_ID_ALL, string_table[StringTable::StringId::CollectionNameAll]},
			{COLLECTION_ID_DEFAULT, string_table[StringTable::StringId::CollectionNameDefault]},
			{COLLECTION_ID_ROOT, string_table[StringTable::StringId::CollectionNameUser]}
	}
	{
		constexpr std::tuple<const wchar_t*, EditMode, int32_t> default_curves[] = {
			{L"linear", EditMode::Bezier, 345633042},
			{L"easeInSine", EditMode::Bezier, -343094698},
			{L"easeOutSine", EditMode::Bezier, 344859039},
			{L"easeInOutSine", EditMode::Bezier, 343215240},
			{L"easeOutInSine", EditMode::Bezier, -343215241},
			{L"easeInQuad", EditMode::Bezier, -342375882},
			{L"easeOutQuad", EditMode::Bezier, 344924375},
			{L"easeInOutQuad", EditMode::Bezier, 342692472},
			{L"easeOutInQuad", EditMode::Bezier, -342692473},
			{L"easeInCubic", EditMode::Bezier, -341264962},
			{L"easeOutCubic", EditMode::Bezier, 343552071},
			{L"easeInOutCubic", EditMode::Bezier, 341385552},
			{L"easeOutInCubic", EditMode::Bezier, -341385553},
			{L"easeInQuart", EditMode::Bezier, -340742168},
			{L"easeOutQuart", EditMode::Bezier, 342375817},
			{L"easeInOutQuart", EditMode::Bezier, 340666746},
			{L"easeOutInQuart", EditMode::Bezier, -340666747},
			{L"easeInQuint", EditMode::Bezier, -340546113},
			{L"easeOutQuint", EditMode::Bezier, 341460956},
			{L"easeInOutQuint", EditMode::Bezier, 340209324},
			{L"easeOutInQuint", EditMode::Bezier, -340209325},
			{L"easeInExpo", EditMode::Bezier, -340154025},
			{L"easeOutExpo", EditMode::Bezier, 341068868},
			{L"easeInOutExpo", EditMode::Bezier, 339947940},
			{L"easeOutInExpo", EditMode::Bezier, -339947941},
			{L"easeInCirc", EditMode::Bezier, -42106373},
			{L"easeOutCirc", EditMode::Bezier, 342044512},
			{L"easeInOutCirc", EditMode::Bezier, 340078632},
			{L"easeOutInCirc", EditMode::Bezier, -340078633},
			{L"easeInBack", EditMode::Bezier, -710932937},
			{L"easeOutBack", EditMode::Bezier, 343296340},
			{L"easeInOutBack", EditMode::Bezier, 737186274},
			{L"easeOutInBack", EditMode::Bezier, -737186275},
			{L"easeInElastic", EditMode::Elastic, -1544896},
			{L"easeOutElastic", EditMode::Elastic, 1544896},
			{L"easeInBounce", EditMode::Bounce, -10758499},
			{L"easeOutBounce", EditMode::Bounce, 10758499}
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

	uint32_t PresetManager::create_collection(const std::wstring& name, bool change_current_id) noexcept {
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
			auto name = ::utf8_to_wide(data.at("name").get<std::string>());
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

	bool PresetManager::export_collection(uint32_t collection_id, const std::filesystem::path& path, bool omit_date, bool set_indent) const noexcept {
		nlohmann::json data = nlohmann::json::object();
		data["presets"] = nlohmann::json::array();
		std::wstring name;
		if (collection_id == COLLECTION_ID_ALL) {
			name = path.stem().wstring();
		}
		else {
			name = get_collection_name(collection_id);
		}
		data["name"] = ::wide_to_utf8(name);
		for (const auto& preset : presets_) {
			if (collection_id == COLLECTION_ID_ALL or collection_id == preset.get_collection_id()) {
				data["presets"].push_back(preset.create_json(omit_date));
			}
		}
		std::ofstream ofs{ path };
		if (!ofs) return false;
		ofs << data.dump(set_indent? 2 : -1);
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

	void PresetManager::rename_preset(uint32_t id, const std::wstring& name) noexcept {
		for (auto& preset : presets_) {
			if (id == preset.get_id()) {
				preset.set_name(name);
				return;
			}
		}
	}

	std::wstring PresetManager::get_preset_name(uint32_t id) const noexcept {
		for (const auto& preset : presets_) {
			if (id == preset.get_id()) {
				return preset.get_name();
			}
		}
		return L"";
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

	bool PresetManager::rename_collection(uint32_t collection_id, const std::wstring& name) noexcept {
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

	std::wstring PresetManager::get_collection_name(uint32_t collection_id) const noexcept {
		for (const auto& [id, name] : collection_info_) {
			if (id == collection_id) {
				return name;
			}
		}
		return L"";
	}

	bool PresetManager::save_json(bool omit_date) const noexcept {
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
					{ "name", ::wide_to_utf8(name)},
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
				data["collections"][0]["presets"].push_back(preset.create_json(omit_date));
				continue;
			}
			auto iter = std::find(custom_collection_ids.begin(), custom_collection_ids.end(), collection_id);
			if (iter == custom_collection_ids.end()) {
				continue;
			}
			auto idx = std::distance(custom_collection_ids.begin(), iter) + 1;
			data["collections"][idx]["presets"].push_back(preset.create_json(omit_date));
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
					auto name = ::utf8_to_wide(collection.at("name").get<std::string>());
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
				preset.get_name(),
				preset.get_date()
				});
		}
		return info;
	}

	bool PresetManager::list_config_from_json(const nlohmann::json& data) noexcept {
		try {
			auto collection_id = data.at("collection_id").get<uint32_t>();
			auto sort_by = data.at("sort_by").get<SortBy>();
			auto sort_order = data.at("sort_order").get<SortOrder>();
			auto filter_info = data.at("filter_info");
			// TODO: この段階ではまだカスタムコレクションが作成されていない
			set_current_collection_id(collection_id);
			set_sort_by(sort_by);
			set_sort_order(sort_order);
			set_filter_info({
				filter_info.at("typeNormal").get<bool>(),
				filter_info.at("typeValue").get<bool>(),
				filter_info.at("typeBezier").get<bool>(),
				filter_info.at("typeElastic").get<bool>(),
				filter_info.at("typeBounce").get<bool>(),
				filter_info.at("typeScript").get<bool>()
			});
		}
		catch (const nlohmann::json::exception&) {
			return false;
		}
		return true;
	}

	void PresetManager::list_config_to_json(nlohmann::json& data) const noexcept {
		data = {
			{ "collection_id", get_current_collection_id() },
			{ "sort_by", get_sort_by() },
			{ "sort_order", get_sort_order() },
			{ "filter_info", {
				{ "typeNormal", get_filter_info().type_normal },
				{ "typeValue", get_filter_info().type_value },
				{ "typeBezier", get_filter_info().type_bezier },
				{ "typeElastic", get_filter_info().type_elastic },
				{ "typeBounce", get_filter_info().type_bounce },
				{ "typeScript", get_filter_info().type_script }
			}}
		};
	}
} // namespace curve_editor::global