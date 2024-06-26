#pragma once

#include "curve_graph.hpp"



namespace cved {
	// カーブ(直線)
	class LinearCurve : public GraphCurve {
	public:
		using GraphCurve::GraphCurve;

		// カーブの値を取得
		double curve_function(double progress, double start, double end) const noexcept override;
		// カーブを初期化
		void clear() noexcept override {}

		void create_data(std::vector<byte>& data) const noexcept override;
		bool load_data(const byte* data, size_t size) noexcept override;
		// ハンドルを描画
		void draw_handle(mkaul::graphics::Graphics*, const View&, float, float, float, float, bool, const mkaul::ColorF&) const noexcept override {}
		
		bool handle_check_hover(
			const mkaul::Point<double>&,
			const GraphView&
		) noexcept override { return false; }
		bool handle_update(
			const mkaul::Point<double>&,
			const GraphView&
		) noexcept override { return false; }
		void handle_end_control() noexcept override {}
		bool is_handle_hovered(const mkaul::Point<double>&, const GraphView&) const noexcept override { return false; }

		ActivePoint pt_check_hover(const mkaul::Point<double>& pt, const GraphView& view) noexcept override;
		bool pt_begin_move(ActivePoint) noexcept override { return true; }
		ActivePoint pt_update(const mkaul::Point<double>& pt, const GraphView& view) noexcept override;
		void pt_end_control() noexcept override;
	};
}