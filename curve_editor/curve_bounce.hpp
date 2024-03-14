#pragma once

#include "curve_graph_numeric.hpp"
#include "handle_bounce.hpp"



namespace cved {
	// カーブ(バウンス)
	class BounceCurve : public NumericGraphCurve {
		static constexpr double DEFAULT_COR = 0.6;
		static constexpr double DEFAULT_PERIOD = 0.5;
		BounceHandle handle_;
		double cor_;
		double period_;

	public:
		// コンストラクタ
		BounceCurve(
			const mkaul::Point<double>& point_start = mkaul::Point{ 0., 0. },
			const mkaul::Point<double>& point_end = mkaul::Point{ 1., 1. },
			bool point_fixed = false,
			GraphCurve* prev = nullptr,
			GraphCurve* next = nullptr,
			double cor = DEFAULT_COR,
			double period = DEFAULT_PERIOD
		);

		// カーブの値を生成
		double get_value(double progress, double start, double end) const noexcept override;

		void clear() noexcept override;

		void reverse() noexcept override;

		int encode() const noexcept override;
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
	};
}