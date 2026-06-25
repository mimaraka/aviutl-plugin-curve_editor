#include "host_object_editor.hpp"

#include <nlohmann/json.hpp>
#include <strconv2.h>

#include "config.hpp"


namespace curve_editor {
	std::wstring EditorHostObject::get_curve_name(uint32_t id) {
		auto curve = global::id_manager.get_curve<Curve>(id);
		if (!curve) {
			return std::wstring();
		}
		return ::sjis_to_wide(curve->get_name().data());
	}

	std::wstring EditorHostObject::get_curve_disp_name(uint32_t id) {
		auto curve = global::id_manager.get_curve<Curve>(id);
		if (!curve) {
			return std::wstring();
		}
		return curve->get_disp_name().data();
	}

	bool EditorHostObject::is_curve_locked(uint32_t id) {
		auto curve = global::id_manager.get_curve<Curve>(id);
		if (!curve) {
			return false;
		}
		return curve->is_locked();
	}

	void EditorHostObject::set_is_curve_locked(uint32_t id, bool locked) {
		auto curve = global::id_manager.get_curve<Curve>(id);
		if (!curve) {
			return;
		}
		curve->set_locked(locked);
	}

	// 現在の編集モード（Normal / Script）で作成済みの全カーブを {idx, id} の配列で返す
	std::wstring EditorHostObject::get_curve_idx_array_as_json() {
		auto data = nlohmann::json::array();
		switch (global::config.get_edit_mode()) {
		case EditMode::Normal:
		{
			auto& ed = global::editor.editor_graph();
			for (size_t i = 0u; i < ed.size_normal(); i++) {
				if (auto curve = ed.p_curve_normal(i)) {
					data.push_back({ { "idx", i }, { "id", curve->get_id() } });
				}
			}
			break;
		}
		case EditMode::Script:
		{
			auto& ed = global::editor.editor_script();
			for (size_t i = 0u; i < ed.size(); i++) {
				if (auto curve = ed.p_curve_script(i)) {
					data.push_back({ { "idx", i }, { "id", curve->get_id() } });
				}
			}
			break;
		}
		default:
			break;
		}
		return ::utf8_to_wide(data.dump());
	}
} // namespace curve_editor