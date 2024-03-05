#pragma once

#include "curve_graph.hpp"



namespace cved {
	class LinearCurve : public GraphCurve {
	public:
		using GraphCurve::GraphCurve;

		double get_value(double progress, double start, double end) const noexcept override;

		void clear() noexcept override {}

		void reverse() noexcept override {}

		void draw_handle(mkaul::graphics::Graphics*, const View&, float, float, float, float, bool, const mkaul::ColorF&) const noexcept override {}

		bool handle_check_hover(
			const mkaul::Point<double>&,
			float,
			const GraphView&
		) noexcept override { return false; }

		bool handle_update(
			const mkaul::Point<double>&,
			const GraphView&
		) noexcept override { return false; }

		void handle_end_control() noexcept override {}

		bool is_handle_hovered(const mkaul::Point<double>& point, float box_width, const GraphView& view) const noexcept override { return false; }

		ActivePoint point_check_hover(const mkaul::Point<double>& point, float box_width, const GraphView& view) noexcept override;
		bool point_begin_move(ActivePoint active_point, const GraphView& view) noexcept override;
		ActivePoint point_update(const mkaul::Point<double>& point, const GraphView& view) noexcept override;
		bool point_move(ActivePoint active_point, const mkaul::Point<double>& point, const GraphView& view) noexcept override;
		void point_end_move() noexcept override {}
		void point_end_control() noexcept override;
	};
}