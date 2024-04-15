#pragma once

#include "curve_graph_numeric.hpp"
#include "handle_bezier.hpp"



namespace cved {
	// カーブ(ベジェ)
	class BezierCurve : public NumericGraphCurve {
		// ハンドル (左)
		BezierHandle handle_left_;
		// ハンドル (右)
		BezierHandle handle_right_;
		// ハンドル (保存用)
		BezierHandle handle_buffer_left_;
		BezierHandle handle_buffer_right_;
		bool flag_prev_move_symmetrically_;

	public:
		// コンストラクタ
		BezierCurve(
			const mkaul::Point<double>& pt_start = mkaul::Point{ 0., 0. },
			const mkaul::Point<double>& pt_end = mkaul::Point{ 1., 1. },
			bool pt_fixed = false,
			GraphCurve* prev = nullptr,
			GraphCurve* next = nullptr,
			const mkaul::Point<double>& handle_left = mkaul::Point<double>{},
			const mkaul::Point<double>& handle_right = mkaul::Point<double>{}
		);

		void set_prev(GraphCurve* p) noexcept override;
		void set_next(GraphCurve* p) noexcept override;

		// カーブの値を取得する
		double curve_function(double progress, double start, double end) const noexcept override;
		void clear() noexcept override;
		void reverse() noexcept override;

		// カーブから一意な整数値を生成
		int32_t encode() const noexcept override;
		// 整数値からカーブに変換
		bool decode(int32_t code) noexcept override;

		void create_data(std::vector<byte>& data) const noexcept override;
		bool load_data(const byte* data, size_t size) noexcept override;

		std::string make_param() const noexcept;

		void draw_handle(
			mkaul::graphics::Graphics* p_graphics,
			const View& view,
			float thickness,
			float root_radius,
			float tip_radius,
			float tip_thickness,
			bool cutoff_line,
			const mkaul::ColorF& color = mkaul::ColorF{}
		) const noexcept override;

		// ハンドルを取得 (左)
		auto handle_left() const noexcept { return const_cast<BezierHandle*>(&handle_left_); }
		// ハンドルを取得 (右)
		auto handle_right() const noexcept { return const_cast<BezierHandle*>(&handle_right_); }

		bool is_handle_hovered(const mkaul::Point<double>& pt, const GraphView& view) const noexcept override;

		// ハンドルの移動を開始
		bool handle_check_hover(
			const mkaul::Point<double>& pt,
			const GraphView& view
		) noexcept override;
		// ハンドルの位置をアップデート
		bool handle_update(
			const mkaul::Point<double>& pt,
			const GraphView& view
		) noexcept override;
		// ハンドルの移動を終了
		void handle_end_control() noexcept override;

		// ポイントの移動を開始
		ActivePoint pt_check_hover(const mkaul::Point<double>& pt, const GraphView& view) noexcept override;
		bool pt_begin_move(ActivePoint active_pt) noexcept override;
		// ポイントの位置をアップデート
		ActivePoint pt_update(const mkaul::Point<double>& pt, const GraphView& view) noexcept override;
		// ポイントを強制的に動かす
		bool pt_move(ActivePoint active_pt, const mkaul::Point<double>& pt) noexcept override;
		// ポイントの移動を終了
		void pt_end_move() noexcept override;
		void pt_end_control() noexcept override;
	};
}