#include "curve_editor.hpp"
#include "config.hpp"



namespace cved {
	namespace global {
		Curve* CurveEditor::current_curve() noexcept {
			switch (global::config.get_edit_mode()) {
			case EditMode::Normal:
			case EditMode::Value:
			case EditMode::Bezier:
			case EditMode::Elastic:
			case EditMode::Bounce:
			case EditMode::Step:
				return editor_graph_.current_curve();

			case EditMode::Script:
			default:
				return editor_script_.curve_script();
			}
		}

		int CurveEditor::track_param() noexcept {
			switch (global::config.get_edit_mode()) {
			case EditMode::Normal:
				return editor_graph_.index_normal() + 1;

			case EditMode::Value:
				return -editor_graph_.index_value() - 1;

			case EditMode::Script:
				return IDCURVE_MAX_N + editor_script_.index() + 1;

			case EditMode::Bezier:
			case EditMode::Elastic:
			case EditMode::Bounce:
			case EditMode::Step:
				return editor_graph_.numeric_curve()->encode();

			default:
				return 0;
			}
		}
	}
}