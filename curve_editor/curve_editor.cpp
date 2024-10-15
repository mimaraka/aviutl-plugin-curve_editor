#include "config.hpp"
#include "constants.hpp"
#include "curve_editor.hpp"



namespace cved {
	namespace global {
		Curve* CurveEditor::current_curve() noexcept {
			switch (global::config.get_edit_mode()) {
			case EditMode::Normal:
			case EditMode::Value:
			case EditMode::Bezier:
			case EditMode::Elastic:
			case EditMode::Bounce:
				return editor_graph_.current_curve();

			case EditMode::Script:
			default:
				return editor_script_.curve_script();
			}
		}

		size_t CurveEditor::current_idx() noexcept {
			switch (global::config.get_edit_mode()) {
			case EditMode::Normal:
				return editor_graph_.idx_normal();

			case EditMode::Value:
				return editor_graph_.idx_value();

			case EditMode::Script:
				return editor_script_.idx();

			default:
				return 0u;
			}
		}

		bool CurveEditor::set_idx(size_t idx) noexcept {
			switch (global::config.get_edit_mode()) {
			case EditMode::Normal:
				return editor_graph_.set_idx_normal(idx);

			case EditMode::Value:
				return editor_graph_.set_idx_value(idx);
				break;

			case EditMode::Script:
				return editor_script_.set_idx(idx);

			default:
				return false;
			}
		}

		bool CurveEditor::advance_idx(int n) noexcept {
			return set_idx((size_t)std::max((int)current_idx() + n, 0));
		}

		void CurveEditor::jump_to_last_idx() noexcept {
			switch (global::config.get_edit_mode()) {
			case EditMode::Normal:
				editor_graph_.jump_to_last_idx_normal();
				break;

			case EditMode::Value:
				editor_graph_.jump_to_last_idx_value();
				break;

			case EditMode::Script:
				editor_script_.jump_to_last_idx();
				break;

			default:
				break;
			}
		}

		bool CurveEditor::is_idx_last() noexcept {
			switch (global::config.get_edit_mode()) {
			case EditMode::Normal:
				return editor_graph_.is_idx_last_normal();

			case EditMode::Value:
				return editor_graph_.is_idx_last_value();

			case EditMode::Script:
				return editor_script_.is_idx_last();

			default:
				return false;
			}
		}

		void CurveEditor::pop_idx() noexcept {
			switch (global::config.get_edit_mode()) {
			case EditMode::Normal:
				editor_graph_.pop_idx_normal();
				break;

			case EditMode::Value:
				editor_graph_.pop_idx_value();
				break;

			case EditMode::Script:
				editor_script_.pop_idx();
				break;

			default:
				break;
			}
		}

		void CurveEditor::reset_id_curves() noexcept {
			editor_graph_.reset_id_curves();
			editor_script_.reset();
		}

		Curve* CurveEditor::get_curve(EditMode mode) noexcept {
			switch (mode) {
			case EditMode::Normal:
			case EditMode::Value:
			case EditMode::Bezier:
			case EditMode::Elastic:
			case EditMode::Bounce:
				return editor_graph_.get_curve(mode);

			case EditMode::Script:
			default:
				return editor_script_.curve_script();
			}
		}

		EditMode CurveEditor::get_mode(const std::string& type_name) noexcept {
			if (type_name == global::CURVE_NAME_BEZIER) return EditMode::Bezier;
			if (type_name == global::CURVE_NAME_NORMAL) return EditMode::Normal;
			if (type_name == global::CURVE_NAME_VALUE) return EditMode::Value;
			if (type_name == global::CURVE_NAME_ELASTIC) return EditMode::Elastic;
			if (type_name == global::CURVE_NAME_BOUNCE) return EditMode::Bounce;
			if (type_name == global::CURVE_NAME_SCRIPT) return EditMode::Script;
			return EditMode::NumEditMode;
		}

		int32_t CurveEditor::track_param() noexcept {
			switch (global::config.get_edit_mode()) {
			case EditMode::Normal:
				return (int32_t)editor_graph_.idx_normal() + 1;

			case EditMode::Value:
				return -(int32_t)editor_graph_.idx_value() - 1;

			case EditMode::Script:
				return (int32_t)(CURVE_ID_MAX + editor_script_.idx()) + 1;

			case EditMode::Bezier:
			case EditMode::Elastic:
			case EditMode::Bounce:
				return editor_graph_.numeric_curve()->encode();

			default:
				return 0;
			}
		}
	}
}