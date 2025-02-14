#pragma once

#include "constants.hpp"
#include "curve_graph_numeric.hpp"
#include "string_table.hpp"



namespace curve_editor {
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

		// コピー代入演算子
		ElasticCurve& operator=(const ElasticCurve& curve) noexcept;

		[[nodiscard]] std::unique_ptr<GraphCurve> clone_graph() const noexcept override { return std::make_unique<ElasticCurve>(*this); }
		[[nodiscard]] std::unique_ptr<Curve> clone() const noexcept override { return clone_graph(); }

		[[nodiscard]] constexpr EditMode get_type() const noexcept override { return EditMode::Elastic; }
		[[nodiscard]] constexpr std::string get_name() const noexcept override { return global::CURVE_NAME_ELASTIC; }
		[[nodiscard]] std::string get_disp_name() const noexcept override { return global::string_table[global::StringTable::StringId::LabelEditModeElastic]; }

		// カーブの値を取得
		[[nodiscard]] double curve_function(double progress, double start, double end) const noexcept override;
		void clear() noexcept override;
		[[nodiscard]] bool is_default() const noexcept override;
		void reverse(bool fix_pt = false) noexcept override;
		[[nodiscard]] bool is_reversed() const noexcept { return reversed_; }

		[[nodiscard]] double get_handle_amp_left_x() const noexcept { return anchor_start().x; }
		[[nodiscard]] double get_handle_amp_left_y() const noexcept;
		[[nodiscard]] double get_handle_amp_right_x() const noexcept { return anchor_end().x; }
		[[nodiscard]] double get_handle_amp_right_y() const noexcept { return get_handle_amp_left_y(); }
		[[nodiscard]] double get_handle_freq_decay_x() const noexcept;
		[[nodiscard]] double get_handle_freq_decay_y() const noexcept;
		[[nodiscard]] double get_handle_freq_decay_root_y() const noexcept;

		void set_handle_amp_left(double y) noexcept;
		void set_handle_amp_right(double y) noexcept { set_handle_amp_left(y); }
		void set_handle_freq_decay(double x, double y) noexcept;

		// カーブから一意な整数値を生成
		[[nodiscard]] int32_t encode() const noexcept override;
		// 整数値からカーブに変換
		bool decode(int32_t code) noexcept override;

		[[nodiscard]] std::string create_params_str(size_t precision = 2) const noexcept override;
		bool read_params(const std::vector<double>& params) noexcept override;

		[[nodiscard]] nlohmann::json create_json() const noexcept override;
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
} // namespace curve_editor

CEREAL_CLASS_VERSION(curve_editor::ElasticCurve, 0)
CEREAL_REGISTER_TYPE(curve_editor::ElasticCurve)
CEREAL_REGISTER_POLYMORPHIC_RELATION(curve_editor::NumericGraphCurve, curve_editor::ElasticCurve)