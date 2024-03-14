#pragma once

#include "curve_graph_numeric.hpp"



namespace cved {
	// カーブ(ステップ)
	class StepCurve : public NumericGraphCurve {
	private:
		

	public:
		StepCurve(
			const mkaul::Point<double>& point_start = mkaul::Point{ 0., 0. },
			const mkaul::Point<double>& point_end = mkaul::Point{ 1., 1. },
			bool point_fixed = false,
			GraphCurve* prev = nullptr,
			GraphCurve* next = nullptr
		) :
			NumericGraphCurve{point_start, point_end, point_fixed, prev, next}
		{}

		// カーブの値を取得
		double get_value(double progress, double start, double end) const noexcept override;

		void clear() noexcept override;

		void reverse() noexcept override;

		// カーブから一意な整数値を生成
		int encode() const noexcept override;
		// 整数値からカーブに変換
		bool decode(int number) noexcept override;

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

		bool is_handle_hovered(const mkaul::Point<double>& point, float box_width, const GraphView& view) const noexcept override;

		bool handle_check_hover(
			const mkaul::Point<double>& point,
			float box_width,
			const GraphView& view
		) noexcept override;

		bool handle_update(
			const mkaul::Point<double>& point,
			const GraphView& view
		) noexcept override;
		void handle_end_control() noexcept override;

		ActivePoint point_check_hover(const mkaul::Point<double>& point, float box_width, const GraphView& view) noexcept override;
		bool point_begin_move(ActivePoint active_point, const GraphView& view) noexcept override;
		ActivePoint point_update(const mkaul::Point<double>& point, const GraphView& view) noexcept override;
		bool point_move(ActivePoint active_point, const mkaul::Point<double>& point) noexcept override;
		void point_end_move() noexcept override;
		void point_end_control() noexcept override;
	};
}