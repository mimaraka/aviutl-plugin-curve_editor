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

			size_t idx_normal_ = 0u;
			size_t idx_value_ = 0u;

			std::unique_ptr<GraphCurve> copied_curve_ = nullptr;

		public:
			GraphCurveEditor();

			void reset_id_curves() noexcept;
			bool load_codes(int32_t code_bezier, int32_t code_elastic, int32_t code_bounce) noexcept;

			auto idx_normal() const noexcept { return idx_normal_; }
			auto idx_value() const noexcept { return idx_value_; }
			bool set_idx_normal(size_t idx) noexcept;
			bool set_idx_value(size_t idx) noexcept;
			void jump_to_last_idx_normal() noexcept;
			void jump_to_last_idx_value() noexcept;
			bool is_idx_normal_last() const noexcept { return idx_normal_ == curves_normal_.size() - 1; }
			bool is_idx_value_last() const noexcept { return idx_value_ == curves_value_.size() - 1; }

			GraphCurve* current_curve() noexcept;
			NumericGraphCurve* numeric_curve() noexcept;
			NormalCurve* curve_normal(size_t idx) noexcept;
			NormalCurve* curve_normal() noexcept { return curve_normal(idx_normal_); }
			ValueCurve* curve_value(size_t idx) noexcept;
			ValueCurve* curve_value() noexcept { return curve_value(idx_value_); }
			BezierCurve* curve_bezier() noexcept { return &curve_bezier_; }
			ElasticCurve* curve_elastic() noexcept { return &curve_elastic_; }
			BounceCurve* curve_bounce() noexcept { return &curve_bounce_; }

			const auto& copied_curve() const noexcept { return copied_curve_; }
			bool is_copying() const noexcept { return copied_curve_.get() != nullptr; }

			void create_data_normal(std::vector<byte>& data) const noexcept;
			bool load_data_normal(const byte* data, size_t size) noexcept;
			// v1.xのデータはNormalCurveに適用される
			bool load_data_v1(const byte* data) noexcept;
			void create_data_value(std::vector<byte>& data) const noexcept;
			bool load_data_value(const byte* data, size_t size) noexcept;
		};
	}
}