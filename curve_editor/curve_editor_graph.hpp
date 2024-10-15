#pragma once

#define NOMINMAX
#include "curve.hpp"
#include "enum.hpp"
#include <cereal/types/vector.hpp>
#include <vector>



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
			auto size_normal() const noexcept { return curves_normal_.size(); }
			auto size_value() const noexcept { return curves_value_.size(); }
			bool set_idx_normal(int idx) noexcept;
			bool set_idx_value(int idx) noexcept;
			bool advance_idx_normal(int n) noexcept { return set_idx_normal((int)idx_normal_ + n); }
			bool advance_idx_value(int n) noexcept { return set_idx_value((int)idx_value_ + n); }
			void jump_to_last_idx_normal() noexcept;
			void jump_to_last_idx_value() noexcept;
			void append_idx_normal() noexcept { curves_normal_.emplace_back(NormalCurve{}); }
			void append_idx_value() noexcept { curves_value_.emplace_back(ValueCurve{}); }
			void pop_idx_normal() noexcept;
			void pop_idx_value() noexcept;
			bool is_idx_first_normal() const noexcept { return idx_normal_ == 0u; }
			bool is_idx_first_value() const noexcept { return idx_value_ == 0u; }
			bool is_idx_last_normal() const noexcept { return idx_normal_ == curves_normal_.size() - 1; }
			bool is_idx_last_value() const noexcept { return idx_value_ == curves_value_.size() - 1; }
			bool is_idx_max_normal() const noexcept { return idx_normal_ == global::CURVE_ID_MAX; }
			bool is_idx_max_value() const noexcept { return idx_value_ == global::CURVE_ID_MAX; }

			GraphCurve* get_curve(EditMode mode) noexcept;
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

			// v1.xのデータはNormalCurveに適用される
			bool load_v1_data(const byte* data) noexcept;

			// TODO: Valueを追加
			template <class Archive>
			void save(Archive& archive, const std::uint32_t) const {
				archive(
					curves_normal_
				);
			}

			template <class Archive>
			void load(Archive& archive, const std::uint32_t) {
				archive(
					curves_normal_
				);
				if (curves_normal_.empty()) {
					curves_normal_.emplace_back(NormalCurve{});
				}
			}
		};
	}
}

CEREAL_CLASS_VERSION(cved::global::GraphCurveEditor, 0)