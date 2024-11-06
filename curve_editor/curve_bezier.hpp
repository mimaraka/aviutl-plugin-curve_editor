#pragma once

#include "constants.hpp"
#include "curve_graph_numeric.hpp"
#include "handle_bezier.hpp"
#include "string_table.hpp"


namespace cved {
	// カーブ(ベジェ)
	class BezierCurve : public NumericGraphCurve {
		BezierHandle handle_;

	public:
		enum class HandleType {
			Left,
			Right
		};

		static constexpr double DEFAULT_HANDLE_XY = 0.3;
		// コンストラクタ
		BezierCurve(
			const mkaul::Point<double>& anchor_start = mkaul::Point{ 0., 0. },
			const mkaul::Point<double>& anchor_end = mkaul::Point{ 1., 1. },
			bool pt_fixed = false,
			GraphCurve* prev = nullptr,
			GraphCurve* next = nullptr,
			mkaul::Point<double> handle_left = mkaul::Point{ DEFAULT_HANDLE_XY, DEFAULT_HANDLE_XY },
			mkaul::Point<double> handle_right = mkaul::Point{ -DEFAULT_HANDLE_XY, -DEFAULT_HANDLE_XY }
		) noexcept;

		// コピーコンストラクタ
		BezierCurve(const BezierCurve& curve) noexcept;

		// カーブの名前を取得する
		[[nodiscard]] constexpr std::string get_name() const noexcept override { return global::CURVE_NAME_BEZIER; }
		[[nodiscard]] std::string get_disp_name() const noexcept override { return global::string_table[global::StringTable::StringId::LabelEditModeBezier]; }

		// アンカーを移動する
		void begin_move_anchor_start(bool bound = false) noexcept override;
		void begin_move_anchor_end(bool bound = false) noexcept override;
		void move_anchor_start(double x, double y, bool forced = false, bool bound = false) noexcept override;
		void move_anchor_end(double x, double y, bool forced = false, bool bound = false) noexcept override;

		// ハンドルの座標を取得する
		[[nodiscard]] auto get_handle_left() const noexcept { return anchor_start() + handle_.left_offset(); }
		[[nodiscard]] auto get_handle_right() const noexcept { return anchor_end() + handle_.right_offset(); }

		// ハンドルを移動する
		void begin_move_handle_left(double scale_x, double scale_y) noexcept;
		void begin_move_handle_right(double scale_x, double scale_y) noexcept;
		void move_handle_left(const mkaul::Point<double>& pt, bool keep_angle = false, bool bound = false) noexcept;
		void move_handle_right(const mkaul::Point<double>& pt, bool keep_angle = false, bool bound = false) noexcept;

		// カーブの値を取得する
		[[nodiscard]] double curve_function(double progress, double start, double end) const noexcept override;
		void clear() noexcept override;
		[[nodiscard]] bool is_default() const noexcept override;
		void reverse(bool fix_pt = false) noexcept override;

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
				handle_
			);
		}

		template <class Archive>
		void load(Archive& archive, const std::uint32_t) {
			archive(
				cereal::base_class<NumericGraphCurve>(this),
				handle_
			);
		}
	};
} // namespace cved

CEREAL_CLASS_VERSION(cved::BezierCurve, 0)
CEREAL_REGISTER_TYPE(cved::BezierCurve)
CEREAL_REGISTER_POLYMORPHIC_RELATION(cved::NumericGraphCurve, cved::BezierCurve)