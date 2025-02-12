#pragma once

#define NOMINMAX
#include "curve.hpp"
#include "enum.hpp"
#include <cereal/types/vector.hpp>
#include <vector>



namespace curve_editor::global {
	class GraphCurveEditor {
	private:
		std::vector<NormalCurve> curves_normal_;
		std::vector<ValueCurve> curves_value_;
		BezierCurve curve_bezier_;
		ElasticCurve curve_elastic_;
		BounceCurve curve_bounce_;

		size_t idx_normal_ = 0u;
		size_t idx_value_ = 0u;

		std::unique_ptr<NormalCurve> copied_curve_normal_;

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
		void append_idx_normal() noexcept { append_curve_normal(NormalCurve{}); }
		void append_idx_value() noexcept { append_curve_value(ValueCurve{}); }
		void append_curve_normal(const NormalCurve& curve) noexcept { curves_normal_.emplace_back(curve); }
		void append_curve_value(const ValueCurve& curve) noexcept { curves_value_.emplace_back(curve); }
		void pop_idx_normal() noexcept;
		void pop_idx_value() noexcept;
		bool is_idx_first_normal() const noexcept { return idx_normal_ == 0u; }
		bool is_idx_first_value() const noexcept { return idx_value_ == 0u; }
		bool is_idx_last_normal() const noexcept { return idx_normal_ == curves_normal_.size() - 1; }
		bool is_idx_last_value() const noexcept { return idx_value_ == curves_value_.size() - 1; }
		bool is_idx_max_normal() const noexcept { return idx_normal_ == global::CURVE_ID_MAX; }
		bool is_idx_max_value() const noexcept { return idx_value_ == global::CURVE_ID_MAX; }

		GraphCurve* get_curve(EditMode mode) noexcept;
		GraphCurve* p_current_curve() noexcept;
		NumericGraphCurve* numeric_curve() noexcept;
		NormalCurve* p_curve_normal(size_t idx) noexcept;
		auto p_curve_normal() noexcept { return p_curve_normal(idx_normal_); }
		ValueCurve* p_curve_value(size_t idx) noexcept;
		auto p_curve_value() noexcept { return p_curve_value(idx_value_); }
		auto& curve_bezier() noexcept { return curve_bezier_; }
		auto& curve_elastic() noexcept { return curve_elastic_; }
		auto& curve_bounce() noexcept { return curve_bounce_; }

		void copy_curve_normal(const NormalCurve& curve) noexcept {
			copied_curve_normal_ = std::make_unique<NormalCurve>(curve);
		}
		bool paste_curve_normal(NormalCurve& curve) noexcept {
			if (is_copying_normal()) {
				curve = *copied_curve_normal_;
				return true;
			}
			return false;
		}
		bool is_copying_normal() const noexcept { return copied_curve_normal_.get() != nullptr; }

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
} // namespace curve_editor::global

CEREAL_CLASS_VERSION(curve_editor::global::GraphCurveEditor, 0)