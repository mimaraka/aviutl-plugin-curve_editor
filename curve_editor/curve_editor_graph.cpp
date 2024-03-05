#include "curve_editor_graph.hpp"
#include "config.hpp"
#include "enum.hpp"



namespace cved {
	namespace global {
		GraphCurveEditor::GraphCurveEditor() :
			curves_normal_{},
			curves_value_{},
			curve_bezier_{ mkaul::Point{0., 0.}, mkaul::Point{1., 1.},true },
			curve_elastic_{ mkaul::Point{0., 0.}, mkaul::Point{1., 0.5}, true },
			curve_bounce_{ mkaul::Point{0., 0.}, mkaul::Point{1., 1.}, true },
			curve_step_{ mkaul::Point{0., 0.}, mkaul::Point{1., 1.}, true },
			index_normal_{ 0 },
			index_value_{ 0 }
		{
			curve_bezier_.clear();
			NormalCurve new_curve{};
			curves_normal_.emplace_back(std::move(new_curve));
		}

		bool GraphCurveEditor::load(int code_bezier, int code_elastic, int code_bounce) noexcept {
			bool result_bezier = curve_bezier_.decode(code_bezier);
			bool result_elastic = curve_elastic_.decode(code_elastic);
			bool result_bounce = curve_bounce_.decode(code_bounce);
			return result_bezier and result_elastic and result_bounce;
		}

		NormalCurve* GraphCurveEditor::curve_normal(size_t index) noexcept {
			if (index < curves_normal_.size()) {
				return &curves_normal_[index];
			}
			else return nullptr;
		}

		ValueCurve* GraphCurveEditor::curve_value(size_t index) noexcept {
			if (index < curves_value_.size()) {
				return &curves_value_[index];
			}
			else return nullptr;
		}

		GraphCurve* GraphCurveEditor::current_curve() noexcept {

			switch (global::config.get_edit_mode()) {
			case EditMode::Bezier:
				return &curve_bezier_;

			case EditMode::Normal:
				return &curves_normal_[index_normal_];

			case EditMode::Value:
				return &curves_value_[index_value_];

			case EditMode::Elastic:
				return &curve_elastic_;

			case EditMode::Bounce:
				return &curve_bounce_;

			case EditMode::Step:
				return &curve_step_;

			default:
				return nullptr;
			}
		}

		NumericGraphCurve* GraphCurveEditor::numeric_curve() noexcept {
			switch (global::config.get_edit_mode()) {
			case EditMode::Bezier:
				return &curve_bezier_;

			case EditMode::Elastic:
				return &curve_elastic_;

			case EditMode::Bounce:
				return &curve_bounce_;

			case EditMode::Step:
				return &curve_step_;

			default:
				return nullptr;
			}
		}
	}
}