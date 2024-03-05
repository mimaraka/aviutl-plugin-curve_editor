#include "drag_and_drop.hpp"
#include "config.hpp"
#include "curve_editor.hpp"
#include "global.hpp"
#include "util.hpp"
#include <mkaul/include/util_hook.hpp>



namespace cved {
	void DragAndDrop::apply_track_mode(ExEdit::Object* p_obj, size_t track_idx) noexcept {
		int16_t type_idx;

		switch (global::config.get_edit_mode()) {
		case EditMode::Elastic:
		case EditMode::Bounce:
		case EditMode::Step:
			type_idx = 1;
			break;

		default:
			type_idx = 0;
		}
		int16_t script_idx_offset = type_idx * (int16_t)ApplyMode::NumApplyMode + (int16_t)global::config.get_apply_mode();

		mkaul::replace_var(&(p_obj->track_mode[track_idx].num), (int16_t)0x0f);
		mkaul::replace_var(
			&(p_obj->track_mode[track_idx].script_idx),
			(int16_t)(util::get_track_script_idx() + script_idx_offset)
		);
		mkaul::replace_var(&(p_obj->track_param[track_idx]), global::editor.track_param());
	}

	bool DragAndDrop::init() noexcept {
		for (int i = 0; i < ExEdit::Object::MAX_TRACK; i++) {
			buttons_[i].init(*global::exedit_internal.p_hwnd_objdialog(), i);
		}
		return true;
	}

	bool DragAndDrop::drag() noexcept {
		if (!dragging_) {
			dragging_ = true;
			return true;
		}
		else return false;
	}

	bool DragAndDrop::drop() noexcept {
		if (dragging_) {
			dragging_ = false;
			for (int i = 0; i < ExEdit::Object::MAX_TRACK; i++) {
				if (buttons_[i].is_visible() and buttons_[i].is_hovered()) {
					auto current_obj_idx = *(global::exedit_internal.p_current_obj_idx());
					auto selected_obj_num = *(global::exedit_internal.p_selected_obj_num());
					auto selected_obj_idcs = global::exedit_internal.array_selected_obj_idcs();
					auto p_array_obj = *(global::exedit_internal.p_array_obj());

					if (p_array_obj) {
						// オブジェクト設定ダイアログが表示されている時
						if (current_obj_idx >= 0) {
							apply_track_mode(&p_array_obj[current_obj_idx], i);
						}
						// オブジェクトが選択されているとき
						for (int j = 0; j < selected_obj_num; j++) {
							apply_track_mode(&p_array_obj[selected_obj_idcs[j]], i);
						}
						// オブジェクト設定ダイアログの更新
						reinterpret_cast<BOOL(__cdecl*)(int)>(global::exedit_internal.base_address() + 0x305e0)(current_obj_idx);
					}
					return true;
				}
			}
		}
		return false;
	}
}