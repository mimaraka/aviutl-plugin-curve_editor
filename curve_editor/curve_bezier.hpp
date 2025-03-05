#pragma once

#include "constants.hpp"
#include "curve_graph_numeric.hpp"
#include "handle_bezier.hpp"
#include "string_table.hpp"


namespace curve_editor {
	// カーブ(ベジェ)
	class BezierCurve : public NumericGraphCurve {
		BezierHandle handle_left_;
		BezierHandle handle_right_;
		mkaul::Point<double> buffer_handle_left_;
		mkaul::Point<double> buffer_handle_right_;

		void on_anchor_begin_move() noexcept;
		void on_anchor_end_move() noexcept;

	public:
		enum class HandleType {
			Left,
			Right
		};

		// コンストラクタ
		BezierCurve(
			const mkaul::Point<double>& anchor_start = mkaul::Point{ 0., 0. },
			const mkaul::Point<double>& anchor_end = mkaul::Point{ 1., 1. },
			bool pt_fixed = false,
			GraphCurve* prev = nullptr,
			GraphCurve* next = nullptr,
			mkaul::Point<double> handle_left = mkaul::Point{ BezierHandle::DEFAULT_HANDLE_RATIO, BezierHandle::DEFAULT_HANDLE_RATIO },
			mkaul::Point<double> handle_right = mkaul::Point{ -BezierHandle::DEFAULT_HANDLE_RATIO, -BezierHandle::DEFAULT_HANDLE_RATIO }
		) noexcept;

		// コピーコンストラクタ
		BezierCurve(const BezierCurve& curve) noexcept;

		// コピー代入演算子
		BezierCurve& operator=(const BezierCurve& curve) noexcept;

		// 等価演算子
		[[nodiscard]] bool operator==(const BezierCurve& curve) const noexcept;

		[[nodiscard]] std::unique_ptr<GraphCurve> clone_graph() const noexcept override { return std::make_unique<BezierCurve>(*this); }
		[[nodiscard]] std::unique_ptr<Curve> clone() const noexcept override { return clone_graph(); }

		[[nodiscard]] constexpr EditMode get_type() const noexcept override { return EditMode::Bezier; }
		[[nodiscard]] constexpr std::string get_name() const noexcept override { return global::CURVE_NAME_BEZIER; }
		[[nodiscard]] std::string get_disp_name() const noexcept override { return global::string_table[global::StringTable::StringId::LabelEditModeBezier]; }

		// アンカーを移動する
		void begin_move_anchor_start(bool bound = false) noexcept override;
		void begin_move_anchor_end(bool bound = false) noexcept override;
		void move_anchor_start(double x, double y, bool forced = false, bool bound = false) noexcept override;
		void move_anchor_end(double x, double y, bool forced = false, bool bound = false) noexcept override;
		void end_move_anchor_start(bool bound = false) noexcept override;
		void end_move_anchor_end(bool bound = false) noexcept override;

		// ハンドルの座標を取得する
		[[nodiscard]] auto get_handle_left() const noexcept { return handle_left_.pos_abs(); }
		[[nodiscard]] auto get_handle_right() const noexcept { return handle_right_.pos_abs(); }

		// ハンドルを移動する
		void begin_move_handle_left(double scale_x, double scale_y, bool bound = false) noexcept;
		void begin_move_handle_right(double scale_x, double scale_y, bool bound = false) noexcept;
		void move_handle_left(const mkaul::Point<double>& pt, bool keep_angle = false, bool bound = false, bool moved_symmetrically = false) noexcept;
		void move_handle_right(const mkaul::Point<double>& pt, bool keep_angle = false, bool bound = false, bool moved_symmetrically = false) noexcept;
		void end_move_handle_left(bool bound = false) noexcept;
		void end_move_handle_right(bool bound = false) noexcept;

		bool is_moving_symmetrically() const noexcept { return handle_left_.is_key_pressed_move_symmetrically() or handle_right_.is_key_pressed_move_symmetrically(); }

		void lock_length_left() noexcept { handle_left_.lock_length(); }
		void unlock_length_left() noexcept { handle_left_.unlock_length(); }
		void lock_length_right() noexcept { handle_right_.lock_length(); }
		void unlock_length_right() noexcept { handle_right_.unlock_length(); }

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
				handle_left_,
				handle_right_
			);
		}

		template <class Archive>
		void load(Archive& archive, const std::uint32_t) {
			archive(
				cereal::base_class<NumericGraphCurve>(this),
				handle_left_,
				handle_right_
			);
		}
	};
} // namespace curve_editor

CEREAL_CLASS_VERSION(curve_editor::BezierCurve, 0)
CEREAL_REGISTER_TYPE(curve_editor::BezierCurve)
CEREAL_REGISTER_POLYMORPHIC_RELATION(curve_editor::NumericGraphCurve, curve_editor::BezierCurve)