#include "curve_editor_script.hpp"



namespace cved {
	namespace global {
		ScriptCurveEditor::ScriptCurveEditor() :
			curves_script_{ ScriptCurve{} },
			idx_{ 0u }
		{}

		void ScriptCurveEditor::reset() noexcept {
			curves_script_.clear();
			curves_script_.emplace_back(ScriptCurve{});
			idx_ = 0u;
		}

		// インデックスを設定する
		bool ScriptCurveEditor::set_idx(int idx) noexcept {
			if (mkaul::in_range(idx, 0, (int)curves_script_.size())) {
				if ((size_t)idx == curves_script_.size()) {
					curves_script_.emplace_back(ScriptCurve{});
				}
				idx_ = idx;
				return true;
			}
			return false;
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
		void ScriptCurveEditor::pop_idx() noexcept {
			if (1u < curves_script_.size()) {
				curves_script_.pop_back();
				if (idx_ == curves_script_.size()) {
					idx_ = curves_script_.size() - 1;
				}
			}
		}
	}
}