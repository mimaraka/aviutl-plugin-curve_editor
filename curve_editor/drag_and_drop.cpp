#include "drag_and_drop.hpp"
#include "config.hpp"
#include "curve_editor.hpp"
#include "global.hpp"
#include "util.hpp"
#include <mkaul/include/util_hook.hpp>



namespace cved {
	// イージングが適用されるトラックバーのインデックスの配列を取得する
	void DragAndDrop::get_applied_track_idcs(int32_t obj_idx, int32_t track_idx, std::vector<int32_t>& ret) noexcept {
		ret.clear();
		ret.emplace_back(track_idx);

		auto offset_idcs = reinterpret_cast<int* (__cdecl*)(int, int)>(global::exedit_internal.base_address() + 0x2ca70u)(
			obj_idx, track_idx
			);
		for (size_t i = 0u; i < 2u; i++) {
			if (offset_idcs[i] != 0) {
				ret.emplace_back(track_idx + offset_idcs[i]);
			}
		}
		// 重複削除
		std::sort(ret.begin(), ret.end());
		ret.erase(std::unique(ret.begin(), ret.end()), ret.end());
	}

	// 指定したトラックバーにCEのイージングを適用する
	void DragAndDrop::apply_easing_to_track(int32_t obj_idx, int32_t track_idx) noexcept {
		int16_t type_idx;
		auto obj_array = *(global::exedit_internal.p_array_obj());
		auto p_obj = &obj_array[obj_idx];

		switch (global::config.get_edit_mode()) {
		case EditMode::Elastic:
		case EditMode::Bounce:
			type_idx = 1;
			break;

		default:
			type_idx = 0;
		}
		int16_t apply_mode = (int16_t)global::config.get_apply_mode(global::config.get_edit_mode());
		int16_t script_idx_offset = type_idx * (int16_t)ApplyMode::NumApplyMode + apply_mode;

		// track_mode, track_paramの該当要素の値を変更
		mkaul::replace_var(&(p_obj->track_mode[track_idx].num), ExEdit::Object::TrackMode::isScript);
		mkaul::replace_var(
			&(p_obj->track_mode[track_idx].script_idx),
			(int16_t)(util::get_track_script_idx() + script_idx_offset)
		);
		mkaul::replace_var(&(p_obj->track_param[track_idx]), global::editor.track_param());
	}

	void DragAndDrop::apply_easing_to_tracks(int32_t obj_idx, int32_t track_idx) noexcept {
		std::vector<int32_t> idcs;
		get_applied_track_idcs(obj_idx, track_idx, idcs);
		for (const auto idx : idcs) {
			apply_easing_to_track(obj_idx, idx);
		}
	}

	bool DragAndDrop::init() noexcept {
		auto hwnd_objdialog = *global::exedit_internal.p_hwnd_objdialog();

		p_graphics_ = mkaul::graphics::Factory::create_graphics();
		if (!p_graphics_) return false;
		p_graphics_->init(hwnd_objdialog);

		for (int i = 0; i < ExEdit::Object::MAX_TRACK; i++) {
			buttons_[i].init(hwnd_objdialog, i);
		}
		return true;
	}

	void DragAndDrop::exit() noexcept {
		if (p_graphics_) {
			p_graphics_->release();
		}
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
					auto obj_array = *(global::exedit_internal.p_array_obj());
					
					if (obj_array) {
						// オブジェクト設定ダイアログが表示されている時
						if (0 <= current_obj_idx) {
							apply_easing_to_tracks(current_obj_idx, i);
						}
						// オブジェクトが選択されている時
						for (int j = 0; j < selected_obj_num; j++) {
							if (i < obj_array[selected_obj_idcs[j]].track_n) {
								apply_easing_to_tracks(selected_obj_idcs[j], i);
							}
						}
						// オブジェクト設定ダイアログの更新
						reinterpret_cast<BOOL(__cdecl*)(int)>(global::exedit_internal.base_address() + 0x305e0u)(current_obj_idx);
					}
					return true;
				}
			}
		}
		return false;
	}

	void DragAndDrop::highlight() const noexcept {
		for (const auto& button : buttons_) {
			if (button.is_visible() and button.is_hovered()) {
				auto current_obj_idx = *(global::exedit_internal.p_current_obj_idx());
				if (0 <= current_obj_idx) {
					std::vector<int32_t> track_idcs;
					get_applied_track_idcs(current_obj_idx, button.track_idx(), track_idcs);
					for (const auto idx : track_idcs) {
						// TODO: ハイライト処理
						//buttons_[idx].highlight(p_graphics_.get());
					}
				}
			}
		}
	}
}