#pragma once

#include "control_point.hpp"
#include "curve_graph.hpp"
#include "view_graph.hpp"



namespace cved {
	class BezierCurve;

	// ベジェのハンドル
	class BezierHandle {
	public:
		enum class Type {
			Left,
			Right
		};

		enum class SnapState {
			Unsnapped,
			SnapStart,
			SnapEnd
		};

	private:
		// ポイント (基準点からのオフセット)
		ControlPoint point_offset_;
		// 右か左か
		const Type type_;
		// ポイントが固定されているかどうか
		bool fixed_;
		// 終点・始点のどちらの辺にスナップしているか
		SnapState snap_state_;
		// 角度固定時に保持する値
		double buffer_angle_;
		// 長さ固定時に保持する値
		double buffer_length_;
		// 角度が固定されているか
		bool locked_angle_;
		// 長さが固定されているか
		bool locked_length_;
		// 過去のフラグ情報
		bool flag_prev_snap_;
		bool flag_prev_lock_angle_;
		bool flag_prev_lock_length_;
		// 反対側のハンドル(整列対象のハンドル)のポインタ
		BezierHandle* handle_opposite_;
		const BezierCurve* const p_curve_;

		// ハンドルの角度を取得
		double get_handle_angle(const GraphView& view) const noexcept;
		// ハンドルの長さを取得
		double get_handle_length(const GraphView& view) const noexcept;

		// 指定したポイントの基準点からの角度を取得
		double get_cursor_angle(
			const mkaul::Point<double>& point,
			const GraphView& view
		) const noexcept;
		// 指定したポイントの基準点からの長さを取得
		double get_cursor_length(
			const mkaul::Point<double>& point,
			const GraphView& view
		) const noexcept;
		// 移動後のハンドルの座標を取得
		auto get_dest_point(
			const mkaul::Point<double>& point,
			const GraphView& view
		) const noexcept;
		// 座標の範囲制限
		void limit_range(
			mkaul::Point<double>* p_point_offset,
			bool keep_angle
		) const noexcept;

		// キーの押下状態を確認
		bool key_state_snap() const noexcept;
		bool key_state_lock_angle() const noexcept;
		bool key_state_lock_length() const noexcept;

		// フラグの立ち上りを検知
		bool check_flag_rise_snap() const noexcept;
		bool check_flag_rise_lock_angle() const noexcept;
		bool check_flag_rise_lock_length() const noexcept;

		// フラグの立ち下がりを検知
		bool check_flag_fall_snap() const noexcept;
		bool check_flag_fall_lock_angle() const noexcept;
		bool check_flag_fall_lock_length() const noexcept;

		void update_flags(const GraphView& view) noexcept;

	public:
		// コンストラクタ
		BezierHandle(
			BezierCurve* p_curve,
			Type type,
			const mkaul::Point<double>& point_offset = mkaul::Point<double>{},
			bool fixed = false,
			BezierHandle* handle_opposite = nullptr
		) :
			p_curve_{ p_curve },
			point_offset_{ point_offset },
			type_{ type },
			fixed_{ fixed },
			handle_opposite_{ handle_opposite },
			snap_state_{ SnapState::Unsnapped },
			buffer_angle_{ 0. },
			buffer_length_{ 0. },
			locked_angle_{ false },
			locked_length_{ false },
			flag_prev_snap_{ false },
			flag_prev_lock_angle_{ false },
			flag_prev_lock_length_{ false }
		{}

		auto handle_opposite() const noexcept { return handle_opposite_; }
		void set_handle_opposite(BezierHandle* p) noexcept { handle_opposite_ = p; }

		// 保持されている角度を取得
		auto buffer_angle() const noexcept { return buffer_angle_; }
		// 保持されている長さを取得
		auto buffer_length() const noexcept { return buffer_length_; }

		// ハンドルをスナップ
		void snap() noexcept;
		// ハンドルのスナップを解除
		void unsnap() noexcept;
		// ハンドルの角度を固定
		void lock_angle(const GraphView& view) noexcept;
		// ハンドルの角度の固定を解除
		void unlock_angle() noexcept;
		// ハンドルの長さを固定
		void lock_length(const GraphView& view) noexcept;
		// ハンドルの長さの固定を解除
		void unlock_length() noexcept;

		// ハンドルのオフセット座標を取得
		const auto& point_offset() const noexcept { return point_offset_.point(); }
		void set_point_offset(const mkaul::Point<double>& point_offset) noexcept { point_offset_.move(point_offset); }

		// カーソルがハンドルにホバーされているかどうか
		bool is_hovered(const mkaul::Point<double>& point, const GraphView& view) const noexcept;

		// ハンドルの移動を開始
		bool check_hover(
			const mkaul::Point<double>& point,
			const GraphView& view
		) noexcept;

		void begin_move(
			const GraphView& view
		) noexcept;

		bool update(
			const mkaul::Point<double>& point,
			const GraphView& view
		) noexcept;

		// ハンドルを動かす
		void move(
			const mkaul::Point<double>& point,
			const GraphView& view,
			bool aligned = false,
			bool moved_symmetrically = false
		) noexcept;

		// ハンドルを強制的に動かす
		void set_position(
			const mkaul::Point<double>& point
		) noexcept;

		// ハンドルの移動を終了
		void end_move() noexcept;
		void end_control() noexcept;
	};
}