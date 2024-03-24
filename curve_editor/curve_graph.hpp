#pragma once

#include "curve.hpp"
#include "control_point.hpp"
#include "view_graph.hpp"



namespace cved {
	// カーブ(グラフ)
	class GraphCurve : public Curve {
	protected:
		ControlPoint point_start_;
		ControlPoint point_end_;
		GraphCurve* prev_;
		GraphCurve* next_;

	public:
		enum class ActivePoint {
			Null,
			Start,
			End
		};

		GraphCurve(
			const mkaul::Point<double>& point_start = mkaul::Point{ 0., 0. },
			const mkaul::Point<double>& point_end = mkaul::Point{ 1., 1. },
			bool point_fixed = false,
			GraphCurve* prev = nullptr,
			GraphCurve* next = nullptr
		) :
			point_start_{ point_start, point_fixed },
			point_end_{ point_end, point_fixed },
			prev_{prev},
			next_{next}
		{}

		auto prev() const noexcept { return prev_; }
		auto next() const noexcept { return next_; }
		virtual void set_prev(GraphCurve* p) noexcept { prev_ = p; }
		virtual void set_next(GraphCurve* p) noexcept { next_ = p; }

		void draw_curve(
			mkaul::graphics::Graphics* p_graphics,
			const View& view,
			float thickness = 1.f,
			float drawing_interval = 1.f,
			const mkaul::ColorF& color = mkaul::ColorF{},
			bool velocity = false
		) const noexcept;

		virtual void draw_handle(
			mkaul::graphics::Graphics* p_graphics,
			const View& view,
			float thickness,
			float root_radius,
			float tip_radius,
			float tip_thickness,
			bool cutoff_line,
			const mkaul::ColorF& color = mkaul::ColorF{}
		) const noexcept = 0;

		void draw_point(
			mkaul::graphics::Graphics* p_graphics,
			const View& view,
			float radius,
			const mkaul::ColorF& color = mkaul::ColorF{}
		) const noexcept;

		const auto& point_start() const noexcept { return point_start_; }
		const auto& point_end() const noexcept { return point_end_; }

		virtual void reverse() noexcept;

		bool is_hovered(const mkaul::Point<double>& point, float box_width, const GraphView& view) const noexcept;
		virtual bool is_point_hovered(const mkaul::Point<double>& point, float box_width, const GraphView& view) const noexcept;
		virtual bool is_handle_hovered(const mkaul::Point<double>& point, float box_width, const GraphView& view) const noexcept = 0;

		virtual bool handle_check_hover(
			const mkaul::Point<double>& point,
			float box_width,
			const GraphView& view
		) noexcept = 0;

		virtual bool handle_update(
			const mkaul::Point<double>& point,
			const GraphView& view
		) noexcept = 0;

		virtual void handle_end_control() noexcept = 0;

		virtual ActivePoint point_check_hover(const mkaul::Point<double>& point, float box_width, const GraphView& view) noexcept = 0;
		virtual bool point_begin_move(ActivePoint active_point) noexcept = 0;
		virtual ActivePoint point_update(const mkaul::Point<double>& point, const GraphView& view) noexcept = 0;
		virtual bool point_move(ActivePoint active_point, const mkaul::Point<double>& point) noexcept;
		virtual void point_end_move() noexcept {};
		virtual void point_end_control() noexcept;
	};
}