#include "preferences_schema.hpp"

#include "config.hpp"
#include "preferences.hpp"
#include <strconv2.h>



namespace curve_editor {
	using StringId = global::StringTable::StringId;

	namespace {
		std::string widget_name(PrefWidget widget) {
			switch (widget) {
			case PrefWidget::Checkbox:	return "checkbox";
			case PrefWidget::Slider:	return "slider";
			case PrefWidget::Combo:		return "combo";
			case PrefWidget::Color:		return "color";
			case PrefWidget::FilePath:	return "filepath";
			case PrefWidget::Text:		return "text";
			}
			return "text";
		}

		std::string resolve(StringId id) {
			return ::wide_to_utf8(std::wstring{ global::string_table[id] });
		}

		// テーマの選択肢(System / Dark / Light)
		std::vector<std::wstring> theme_options() {
			return {
				global::string_table[StringId::ThemeSystem],
				global::string_table[StringId::ThemeDark],
				global::string_table[StringId::ThemeLight]
			};
		}

		// 言語の選択肢(自動 + 各言語のエンドニム)
		std::vector<std::wstring> language_options() {
			return {
				global::string_table[StringId::WordAutomatic],
				L"日本語",
				L"English",
				L"Deutsch",
				L"Bahasa Indonesia",
				L"한국어",
				L"简体中文"
			};
		}
	} // namespace

	const std::vector<PrefDescriptor>& pref_schema() {
		static const std::vector<PrefDescriptor> schema = [] {
			std::vector<PrefDescriptor> s;

			// --- General ---
			s.push_back(PrefDescriptor{
				.key = "language",
				.widget = PrefWidget::Combo,
				.label = StringId::PrefLabelLanguage,
				.category = StringId::PreferencesCategoryGeneral,
				.options = language_options(),
				.getter = []() -> nlohmann::json { return (int)global::config.get_language(); },
				.setter = [](const nlohmann::json& v) { global::config.set_language((Language)v.get<int>()); }
			});
			s.push_back(PrefDescriptor{
				.key = "notify_update",
				.widget = PrefWidget::Checkbox,
				.label = StringId::PrefLabelNotifyUpdate,
				.category = StringId::PreferencesCategoryGeneral,
				.getter = []() -> nlohmann::json { return global::config.get_notify_update(); },
				.setter = [](const nlohmann::json& v) { global::config.set_notify_update(v.get<bool>()); }
			});

			// --- Appearance ---
			s.push_back(PrefDescriptor{
				.key = "theme",
				.widget = PrefWidget::Combo,
				.label = StringId::PrefLabelTheme,
				.category = StringId::PreferencesCategoryAppearance,
				.options = theme_options(),
				.getter = []() -> nlohmann::json { return (int)global::config.get_theme(); },
				.setter = [](const nlohmann::json& v) { global::config.set_theme((ThemeId)v.get<int>()); }
			});
			s.push_back(PrefDescriptor{
				.key = "curve_color",
				.widget = PrefWidget::Color,
				.label = StringId::PrefLabelCurveColor,
				.category = StringId::PreferencesCategoryAppearance,
				.getter = []() -> nlohmann::json { return (uint32_t)global::config.get_curve_color().colorref(); },
				.setter = [](const nlohmann::json& v) { global::config.set_curve_color(static_cast<COLORREF>(v.get<uint32_t>())); }
			});
			s.push_back(PrefDescriptor{
				.key = "curve_thickness",
				.widget = PrefWidget::Slider,
				.label = StringId::PrefLabelCurveThickness,
				.category = StringId::PreferencesCategoryAppearance,
				.min = 0.1, .max = 10.0, .step = 0.1,
				.getter = []() -> nlohmann::json { return global::config.get_curve_thickness(); },
				.setter = [](const nlohmann::json& v) { global::config.set_curve_thickness(v.get<float>()); }
			});
			s.push_back(PrefDescriptor{
				.key = "curve_resolution",
				.widget = PrefWidget::Slider,
				.label = StringId::PrefLabelCurveResolution,
				.category = StringId::PreferencesCategoryAppearance,
				.min = 100.0, .max = 1000.0, .step = 1.0,
				.getter = []() -> nlohmann::json { return global::config.get_curve_resolution(); },
				.setter = [](const nlohmann::json& v) { global::config.set_curve_resolution(v.get<uint32_t>()); }
			});
			s.push_back(PrefDescriptor{
				.key = "show_bg_image",
				.widget = PrefWidget::Checkbox,
				.label = StringId::PrefLabelShowBgImage,
				.category = StringId::PreferencesCategoryAppearance,
				.getter = []() -> nlohmann::json { return global::config.get_show_bg_image(); },
				.setter = [](const nlohmann::json& v) { global::config.set_show_bg_image(v.get<bool>()); }
			});
			s.push_back(PrefDescriptor{
				.key = "bg_image_path",
				.widget = PrefWidget::FilePath,
				.label = StringId::PrefLabelBgImagePath,
				.category = StringId::PreferencesCategoryAppearance,
				.getter = []() -> nlohmann::json { return ::sjis_to_utf8(global::config.get_bg_image_path().string()); },
				.setter = [](const nlohmann::json& v) { global::config.set_bg_image_path(std::filesystem::path(::utf8_to_sjis(v.get<std::string>()))); }
			});
			s.push_back(PrefDescriptor{
				.key = "bg_image_opacity",
				.widget = PrefWidget::Slider,
				.label = StringId::PrefLabelBgImageOpacity,
				.category = StringId::PreferencesCategoryAppearance,
				.min = 0.0, .max = 1.0, .step = 0.01,
				.getter = []() -> nlohmann::json { return global::config.get_bg_image_opacity(); },
				.setter = [](const nlohmann::json& v) { global::config.set_bg_image_opacity(v.get<float>()); }
			});
			s.push_back(PrefDescriptor{
				.key = "apply_button_height",
				.widget = PrefWidget::Slider,
				.label = StringId::PrefLabelApplyButtonHeight,
				.category = StringId::PreferencesCategoryAppearance,
				.min = 30.0, .max = 200.0, .step = 1.0,
				.getter = []() -> nlohmann::json { return global::config.get_apply_button_height(); },
				.setter = [](const nlohmann::json& v) { global::config.set_apply_button_height(v.get<uint32_t>()); }
			});
			s.push_back(PrefDescriptor{
				.key = "show_trace",
				.widget = PrefWidget::Checkbox,
				.label = StringId::PrefLabelShowTrace,
				.category = StringId::PreferencesCategoryAppearance,
				.getter = []() -> nlohmann::json { return global::config.get_show_trace(); },
				.setter = [](const nlohmann::json& v) { global::config.set_show_trace(v.get<bool>()); }
			});
			s.push_back(PrefDescriptor{
				.key = "enable_animation",
				.widget = PrefWidget::Checkbox,
				.label = StringId::PrefLabelEnableAnimation,
				.category = StringId::PreferencesCategoryAppearance,
				.getter = []() -> nlohmann::json { return global::config.get_enable_animation(); },
				.setter = [](const nlohmann::json& v) { global::config.set_enable_animation(v.get<bool>()); }
			});

			// --- Behavior ---
			s.push_back(PrefDescriptor{
				.key = "show_popup",
				.widget = PrefWidget::Checkbox,
				.label = StringId::PrefLabelShowPopup,
				.category = StringId::PreferencesCategoryBehavior,
				.getter = []() -> nlohmann::json { return global::config.get_show_popup(); },
				.setter = [](const nlohmann::json& v) { global::config.set_show_popup(v.get<bool>()); }
			});
			s.push_back(PrefDescriptor{
				.key = "enable_hotkeys",
				.widget = PrefWidget::Checkbox,
				.label = StringId::PrefLabelEnableHotkeys,
				.category = StringId::PreferencesCategoryBehavior,
				.getter = []() -> nlohmann::json { return global::config.get_enable_hotkeys(); },
				.setter = [](const nlohmann::json& v) { global::config.set_enable_hotkeys(v.get<bool>()); }
			});

			// --- Editing ---
			s.push_back(PrefDescriptor{
				.key = "invert_wheel",
				.widget = PrefWidget::Checkbox,
				.label = StringId::PrefLabelInvertWheel,
				.category = StringId::PreferencesCategoryEditing,
				.getter = []() -> nlohmann::json { return global::config.get_invert_wheel(); },
				.setter = [](const nlohmann::json& v) { global::config.set_invert_wheel(v.get<bool>()); }
			});
			s.push_back(PrefDescriptor{
				.key = "auto_copy",
				.widget = PrefWidget::Checkbox,
				.label = StringId::PrefLabelAutoCopy,
				.category = StringId::PreferencesCategoryEditing,
				.getter = []() -> nlohmann::json { return global::config.get_auto_copy(); },
				.setter = [](const nlohmann::json& v) { global::config.set_auto_copy(v.get<bool>()); }
			});
			s.push_back(PrefDescriptor{
				.key = "auto_apply",
				.widget = PrefWidget::Checkbox,
				.label = StringId::PrefLabelAutoApply,
				.category = StringId::PreferencesCategoryEditing,
				.getter = []() -> nlohmann::json { return global::config.get_auto_apply(); },
				.setter = [](const nlohmann::json& v) { global::config.set_auto_apply(v.get<bool>()); }
			});
			s.push_back(PrefDescriptor{
				.key = "word_wrap",
				.widget = PrefWidget::Checkbox,
				.label = StringId::PrefLabelWordWrap,
				.category = StringId::PreferencesCategoryEditing,
				.getter = []() -> nlohmann::json { return global::config.get_word_wrap(); },
				.setter = [](const nlohmann::json& v) { global::config.set_word_wrap(v.get<bool>()); }
			});

			return s;
		}();
		return schema;
	}

	nlohmann::json build_schema_json() {
		nlohmann::json arr = nlohmann::json::array();
		for (const auto& d : pref_schema()) {
			nlohmann::json item;
			item["key"] = d.key;
			item["widget"] = widget_name(d.widget);
			item["label"] = resolve(d.label);
			item["category"] = resolve(d.category);
			if (d.widget == PrefWidget::Slider) {
				item["min"] = d.min;
				item["max"] = d.max;
				item["step"] = d.step;
			}
			if (!d.options.empty()) {
				nlohmann::json opts = nlohmann::json::array();
				for (const auto& opt : d.options) {
					opts.push_back(::wide_to_utf8(opt));
				}
				item["options"] = opts;
			}
			arr.push_back(item);
		}
		return arr;
	}

	nlohmann::json build_values_json() {
		nlohmann::json obj = nlohmann::json::object();
		for (const auto& d : pref_schema()) {
			obj[d.key] = d.getter();
		}
		return obj;
	}

	void apply_values_json(const nlohmann::json& values) {
		for (const auto& d : pref_schema()) {
			if (values.contains(d.key)) {
				try {
					d.setter(values.at(d.key));
				}
				catch (const nlohmann::json::exception&) {}
			}
		}
	}

	nlohmann::json build_default_values_json() {
		Preferences tmp;
		tmp.reset();
		nlohmann::json json;
		tmp.to_json(json);
		return json;
	}
} // namespace curve_editor
