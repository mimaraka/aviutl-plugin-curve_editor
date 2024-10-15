#pragma once

#include "constants.hpp"
#include "curve_graph_numeric.hpp"



namespace cved {
	// カーブ(振動)
	class ElasticCurve : public NumericGraphCurve {
		static constexpr double DEFAULT_AMP = 1.;
		static constexpr double DEFAULT_FREQ = 5.;
		static constexpr double DEFAULT_DECAY = 6.;
		double amplitude_;
		double frequency_;
		double decay_;
		bool reversed_;

	public:
		// コンストラクタ
		ElasticCurve(
			const mkaul::Point<double>& anchor_start = mkaul::Point{ 0., 0. },
			const mkaul::Point<double>& anchor_end = mkaul::Point{ 1., 1. },
			bool pt_fixed = false,
			GraphCurve* prev = nullptr,
			GraphCurve* next = nullptr,
			double amp = DEFAULT_AMP,
			double freq = DEFAULT_FREQ,
			double decay = DEFAULT_DECAY,
			bool reversed = false
		) noexcept;

		// コピーコンストラクタ
		ElasticCurve(const ElasticCurve& curve) noexcept;

		constexpr std::string get_type() const noexcept override { return global::CURVE_NAME_ELASTIC; }

		// カーブの値を取得
		double curve_function(double progress, double start, double end) const noexcept override;
		void clear() noexcept override;
		bool is_default() const noexcept override;
		void reverse(bool fix_pt = false) noexcept override;
		bool is_reversed() const noexcept { return reversed_; }

		auto get_amplitude() const noexcept { return amplitude_; }
		auto get_frequency() const noexcept { return frequency_; }
		auto get_decay() const noexcept { return decay_; }

		double get_handle_amp_left_x() const noexcept { return anchor_start().x; }
		double get_handle_amp_left_y() const noexcept;
		double get_handle_amp_right_x() const noexcept { return anchor_end().x; }
		double get_handle_amp_right_y() const noexcept { return get_handle_amp_left_y(); }
		double get_handle_freq_decay_x() const noexcept;
		double get_handle_freq_decay_y() const noexcept;
		double get_handle_freq_decay_root_y() const noexcept;

		void set_handle_amp_left(double y) noexcept;
		void set_handle_amp_right(double y) noexcept { set_handle_amp_left(y); }
		void set_handle_freq_decay(double x, double y) noexcept;

		// カーブから一意な整数値を生成
		int32_t encode() const noexcept override;
		// 整数値からカーブに変換
		bool decode(int32_t code) noexcept override;

		nlohmann::json create_json() const noexcept override;
		bool load_json(const nlohmann::json& data) noexcept override;

		template <class Archive>
		void save(Archive& archive, const std::uint32_t) const {
			archive(
				cereal::base_class<NumericGraphCurve>(this),
				amplitude_,
				frequency_,
				decay_
			);
		}

		template <class Archive>
		void load(Archive& archive, const std::uint32_t) {
			archive(
				cereal::base_class<NumericGraphCurve>(this),
				amplitude_,
				frequency_,
				decay_
			);
		}
	};
}

CEREAL_CLASS_VERSION(cved::ElasticCurve, 0)
CEREAL_REGISTER_TYPE(cved::ElasticCurve)
CEREAL_REGISTER_POLYMORPHIC_RELATION(cved::NumericGraphCurve, cved::ElasticCurve)