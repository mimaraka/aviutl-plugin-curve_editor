#include "curve_graph.hpp"



namespace cved {
	double GraphCurve::get_value(double progress, double start, double end) const noexcept {
		CurveFunction func_ret = [this](double prog, double st, double ed) {return curve_function(prog, st, ed); };
		for (const auto& modifier : modifiers_) {
			func_ret = modifier->apply(func_ret);
		}
		return func_ret(progress, start, end);
	}

	void GraphCurve::draw_curve(
		mkaul::graphics::Graphics* p_graphics,
		const View& view,
		float thickness,
		float drawing_interval,
		const mkaul::ColorF& color,
		bool velocity
	) const noexcept {
		mkaul::WindowRectangle rect_wnd;
		p_graphics->get_rect(&rect_wnd);

		const float client_from_x = std::max(view.view_to_client_x(0., rect_wnd), 0.f);
		for (
			float x = client_from_x;
			x < rect_wnd.get_width() and view.client_to_view_x(x, rect_wnd) < 1.;
			x += drawing_interval
			) {
			auto view_from_x = view.client_to_view_x(x, rect_wnd);
			auto view_to_x = std::min(view.client_to_view_x(x + drawing_interval, rect_wnd), 1.);
			double view_from_y, view_to_y;
			if (velocity) {
				view_from_y = get_velocity(view_from_x, 0., 1.);
				view_to_y = get_velocity(view_to_x, 0., 1.);
			}
			else {
				view_from_y = get_value(view_from_x, 0., 1.);
				view_to_y = get_value(view_to_x, 0., 1.);
			}
			auto view_from = mkaul::Point{ view_from_x, view_from_y };
			auto view_to = mkaul::Point{ view_to_x, view_to_y };
			auto client_from = view.view_to_client(view_from, rect_wnd);
			auto client_to = view.view_to_client(view_to, rect_wnd);
			p_graphics->draw_line(client_from, client_to, color, mkaul::graphics::Stroke{ thickness });
		}
	}

	void GraphCurve::draw_pt(
		mkaul::graphics::Graphics* p_graphics,
		const View& view,
		float radius,
		const mkaul::ColorF& color
	) const noexcept {
		mkaul::WindowRectangle rect_wnd;
		p_graphics->get_rect(&rect_wnd);

		auto client_start = view.view_to_client(pt_start_.pt(), rect_wnd);
		auto client_end = view.view_to_client(pt_end_.pt(), rect_wnd);
		p_graphics->fill_ellipse(
			client_start,
			radius, radius, color
		);
		p_graphics->fill_ellipse(
			client_end,
			radius, radius, color
		);
	}

	void GraphCurve::reverse(bool fix_pt) noexcept {
		if (!fix_pt) {
			auto pt_tmp = pt_start_.pt();
			pt_start_ = mkaul::Point{ 1., 1. } - pt_end_.pt();
			pt_end_ = mkaul::Point{ 1., 1. } - pt_tmp;
		}
	}

	bool GraphCurve::is_hovered(const mkaul::Point<double>& pt, const GraphView& view) const noexcept {
		return is_pt_hovered(pt, view) or is_handle_hovered(pt, view);
	}

	bool GraphCurve::is_pt_hovered(const mkaul::Point<double>& pt, const GraphView& view) const noexcept {
		return pt_start_.is_hovered(pt, view) or pt_end_.is_hovered(pt, view);
	}

	bool GraphCurve::pt_move(ActivePoint active_pt, const mkaul::Point<double>& pt) noexcept {
		switch (active_pt) {
		case ActivePoint::Start:
			pt_start_.move(mkaul::Point{ std::min(pt.x, pt_end_.x()), pt.y });
			break;

		case ActivePoint::End:
			pt_end_.move(mkaul::Point{ std::max(pt.x, pt_start_.x()), pt.y });
			break;

		default:
			return false;
		}
		return true;
	}

	void GraphCurve::pt_end_control() noexcept {
		pt_start_.end_control();
		pt_end_.end_control();
	}

	Modifier* GraphCurve::get_modifier(size_t idx) const noexcept {
		if (idx < modifiers_.size()) {
			return modifiers_[idx].get();
		}
		else return nullptr;
	}

	bool GraphCurve::remove_modifier(size_t idx) noexcept {
		if (idx < modifiers_.size()) {
			modifiers_.erase(modifiers_.begin() + idx);
			return true;
		}
		else return false;
	}
}