#pragma once

#include "constants.hpp"
#include "curve_graph_numeric.hpp"
#include "string_table.hpp"



namespace curve_editor {
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

		// コピー代入演算子
		BounceCurve& operator=(const BounceCurve& curve) noexcept;

		// 等価演算子
		[[nodiscard]] bool operator==(const BounceCurve& curve) const noexcept;

		[[nodiscard]] std::unique_ptr<GraphCurve> clone_graph() const noexcept override { return std::make_unique<BounceCurve>(*this); }
		[[nodiscard]] std::unique_ptr<Curve> clone() const noexcept override { return clone_graph(); }

		[[nodiscard]] constexpr EditMode get_type() const noexcept override { return EditMode::Bounce; }
		[[nodiscard]] constexpr std::string get_name() const noexcept override { return global::CURVE_NAME_BOUNCE; }
		[[nodiscard]] std::string get_disp_name() const noexcept override { return global::string_table[global::StringTable::StringId::LabelEditModeBounce]; }

		// カーブの値を生成
		[[nodiscard]] double curve_function(double progress, double start, double end) const noexcept override;
		void clear() noexcept override;
		[[nodiscard]] bool is_default() const noexcept override;
		void reverse(bool fix_pt = false) noexcept override;
		[[nodiscard]] bool is_reversed() const noexcept { return reversed_; }

		[[nodiscard]] double get_handle_x() const noexcept;
		[[nodiscard]] double get_handle_y() const noexcept;
		void set_handle(double x, double y) noexcept;

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
} // namespace curve_editor

CEREAL_CLASS_VERSION(curve_editor::BounceCurve, 0)
CEREAL_REGISTER_TYPE(curve_editor::BounceCurve)
CEREAL_REGISTER_POLYMORPHIC_RELATION(curve_editor::GraphCurve, curve_editor::BounceCurve)