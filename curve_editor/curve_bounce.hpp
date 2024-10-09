#pragma once

#include "curve_graph_numeric.hpp"



namespace cved {
	// カーブ(バウンス)
	class BounceCurve : public NumericGraphCurve {
		static constexpr double DEFAULT_COR = 0.6;
		static constexpr double DEFAULT_PERIOD = 0.5;
		double cor_;
		double period_;
		bool reversed_;

	public:
		// コンストラクタ
		BounceCurve(
			const mkaul::Point<double>& anchor_start = mkaul::Point{ 0., 0. },
			const mkaul::Point<double>& anchor_end = mkaul::Point{ 1., 1. },
			bool pt_fixed = false,
			GraphCurve* prev = nullptr,
			GraphCurve* next = nullptr,
			double cor = DEFAULT_COR,
			double period = DEFAULT_PERIOD,
			bool reversed = false
		);
		~BounceCurve() {}

		// コピーコンストラクタ
		BounceCurve(const BounceCurve& curve) noexcept;

		std::string get_type() const noexcept override { return "bounce"; }

		// カーブの値を生成
		double curve_function(double progress, double start, double end) const noexcept override;
		void clear() noexcept override;
		bool is_default() const noexcept override;
		void reverse(bool fix_pt = false) noexcept override;
		bool is_reversed() const noexcept { return reversed_; }

		auto get_cor() const noexcept { return cor_; }
		auto get_period() const noexcept { return period_; }

		double get_handle_x() const noexcept;
		double get_handle_y() const noexcept;
		void set_handle(double x, double y) noexcept;

		// カーブから一意な整数値を生成
		int32_t encode() const noexcept override;
		// 整数値からカーブに変換
		bool decode(int32_t code) noexcept override;

		template <class Archive>
		void save(Archive& archive, const std::uint32_t) const {
			archive(
				cereal::base_class<NumericGraphCurve>(this),
				cor_,
				period_
			);
		}

		template <class Archive>
		void load(Archive& archive, const std::uint32_t) {
			archive(
				cereal::base_class<NumericGraphCurve>(this),
				cor_,
				period_
			);
		}
	};
}

CEREAL_CLASS_VERSION(cved::BounceCurve, 0)
CEREAL_REGISTER_TYPE(cved::BounceCurve)
CEREAL_REGISTER_POLYMORPHIC_RELATION(cved::GraphCurve, cved::BounceCurve)