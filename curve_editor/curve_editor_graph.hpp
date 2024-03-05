#pragma once

#define NOMINMAX
#include <vector>
#include "curve.hpp"
#include "curve_graph.hpp"
#include "curve_graph_numeric.hpp"
#include "curve_normal.hpp"
#include "curve_value.hpp"
#include "curve_bezier.hpp"
#include "curve_elastic.hpp"
#include "curve_bounce.hpp"
#include "curve_linear.hpp"
#include "curve_step.hpp"
#include "enum.hpp"



namespace cved {
	namespace global {
		class GraphCurveEditor {
		private:
			std::vector<NormalCurve> curves_normal_;
			std::vector<ValueCurve> curves_value_;
			BezierCurve curve_bezier_;
			ElasticCurve curve_elastic_;
			BounceCurve curve_bounce_;
			StepCurve curve_step_;

			int index_normal_;
			int index_value_;

			std::unique_ptr<GraphCurve> copied_curve_ = nullptr;

		public:
			GraphCurveEditor();
			bool load(int code_bezier, int code_elastic, int code_bounce) noexcept;

			int index_normal() const noexcept { return index_normal_; }
			int index_value() const noexcept { return index_value_; }
			GraphCurve* current_curve() noexcept;
			NumericGraphCurve* numeric_curve() noexcept;
			NormalCurve* curve_normal(size_t index) noexcept;
			NormalCurve* curve_normal() noexcept { return curve_normal(index_normal_); }
			ValueCurve* curve_value(size_t index) noexcept;
			ValueCurve* curve_value() noexcept { return curve_value(index_value_); }
			BezierCurve* curve_bezier() noexcept { return &curve_bezier_; }
			ElasticCurve* curve_elastic() noexcept { return &curve_elastic_; }
			BounceCurve* curve_bounce() noexcept { return &curve_bounce_; }
			StepCurve* curve_step() noexcept { return &curve_step_; }
			const auto& copied_curve() const noexcept { return copied_curve_; }
			bool is_copying() const noexcept { return copied_curve_.get() != nullptr; }

		};
	}
}