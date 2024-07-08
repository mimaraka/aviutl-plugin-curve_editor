#include "curve_editor_script.hpp"



namespace cved {
	namespace global {
		void ScriptCurveEditor::reset() noexcept {
			idx_ = 0u;
		}

		// インデックスを設定する
		bool ScriptCurveEditor::set_idx(size_t idx) noexcept {
			if (idx < curves_script_.size()) {
				idx_ = idx;
				return true;
			}
			else if (idx == curves_script_.size()) {
				curves_script_.emplace_back(ScriptCurve{});
				idx_ = idx;
				return true;
			}
			else return false;
		}

		// スクリプトカーブを取得する
		ScriptCurve* ScriptCurveEditor::curve_script(size_t idx) noexcept {
			if (idx < curves_script_.size()) {
				return &curves_script_[idx];
			}
			else return nullptr;
		}

		// 最後のインデックスにジャンプする
		void ScriptCurveEditor::jump_to_last_idx() noexcept {
			idx_ = curves_script_.size() - 1;
		}

		// 最後のインデックスを削除する
		void ScriptCurveEditor::delete_last_idx() noexcept {
			if (1u < curves_script_.size()) {
				curves_script_.pop_back();
				if (idx_ == curves_script_.size()) {
					idx_ = curves_script_.size() - 1;
				}
			}
		}
	}
}