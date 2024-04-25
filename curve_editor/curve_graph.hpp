#pragma once

#include "curve.hpp"
#include "control_point.hpp"
#include "view_graph.hpp"



namespace cved {
	// カーブ(グラフ)
	class GraphCurve : public Curve {
		GraphCurve* prev_;
		GraphCurve* next_;
		uint32_t sampling_resolution_ = 0u;
		uint32_t quantization_resolution_ = 0u;

	protected:
		ControlPoint pt_start_;
		ControlPoint pt_end_;

	public:
		enum class ActivePoint {
			Null,
			Start,
			End
		};

		GraphCurve(
			const mkaul::Point<double>& pt_start = mkaul::Point{ 0., 0. },
			const mkaul::Point<double>& pt_end = mkaul::Point{ 1., 1. },
			bool pt_fixed = false,
			GraphCurve* prev = nullptr,
			GraphCurve* next = nullptr
		) :
			pt_start_{ pt_start, pt_fixed },
			pt_end_{ pt_end, pt_fixed },
			prev_{prev},
			next_{next}
		{}

		auto prev() const noexcept { return prev_; }
		auto next() const noexcept { return next_; }
		virtual void set_prev(GraphCurve* p) noexcept { prev_ = p; }
		virtual void set_next(GraphCurve* p) noexcept { next_ = p; }

		auto get_sampling_resolution() const noexcept { return sampling_resolution_; }
		void set_sampling_resolution(uint32_t sampling_resolution) noexcept { sampling_resolution_ = sampling_resolution; }
		auto get_quantization_resolution() const noexcept { return quantization_resolution_; }
		void set_quantization_resolution(uint32_t quantization_resolution) noexcept { quantization_resolution_ = quantization_resolution; }

		double get_value(double progress, double start, double end) const noexcept override;

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

		void draw_pt(
			mkaul::graphics::Graphics* p_graphics,
			const View& view,
			float radius,
			const mkaul::ColorF& color = mkaul::ColorF{}
		) const noexcept;

		const auto& pt_start() const noexcept { return pt_start_; }
		const auto& pt_end() const noexcept { return pt_end_; }

		virtual void reverse(bool fix_pt = false) noexcept;

		bool is_hovered(const mkaul::Point<double>& pt, const GraphView& view) const noexcept;
		virtual bool is_pt_hovered(const mkaul::Point<double>& pt, const GraphView& view) const noexcept;
		virtual bool is_handle_hovered(const mkaul::Point<double>& pt, const GraphView& view) const noexcept = 0;

		virtual bool handle_check_hover(
			const mkaul::Point<double>& pt,
			const GraphView& view
		) noexcept = 0;

		virtual bool handle_update(
			const mkaul::Point<double>& pt,
			const GraphView& view
		) noexcept = 0;

		virtual void handle_end_control() noexcept = 0;

		virtual ActivePoint pt_check_hover(const mkaul::Point<double>& pt, const GraphView& view) noexcept = 0;
		virtual bool pt_begin_move(ActivePoint active_pt) noexcept = 0;
		virtual ActivePoint pt_update(const mkaul::Point<double>& pt, const GraphView& view) noexcept = 0;
		virtual bool pt_move(ActivePoint active_pt, const mkaul::Point<double>& pt) noexcept;
		virtual void pt_end_move() noexcept {};
		virtual void pt_end_control() noexcept;
	};
}