#include "curve_editor_script.hpp"



namespace cved {
	namespace global {
		void ScriptCurveEditor::reset() noexcept {
			index_ = 0u;
		}

		// インデックスを設定する
		bool ScriptCurveEditor::set_idx(size_t idx) noexcept {
			if (idx < curves_script_.size()) {
				index_ = idx;
				return true;
			}
			else if (idx == curves_script_.size()) {
				curves_script_.emplace_back(ScriptCurve{});
				index_ = idx;
				return true;
			}
			else return false;
		}

		// 最後のインデックスにジャンプする
		void ScriptCurveEditor::jump_to_last_idx() noexcept {
			index_ = curves_script_.size() - 1;
		}

		// 最後のインデックスを削除する
		void ScriptCurveEditor::delete_last_idx() noexcept {
			if (1u < curves_script_.size()) {
				curves_script_.pop_back();
				if (index_ == curves_script_.size()) {
					index_ = curves_script_.size() - 1;
				}
			}
		}
	}
}