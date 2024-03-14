#include "curve_graph.hpp"



namespace cved {
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

	void GraphCurve::draw_point(
		mkaul::graphics::Graphics* p_graphics,
		const View& view,
		float radius,
		const mkaul::ColorF& color
	) const noexcept {
		mkaul::WindowRectangle rect_wnd;
		p_graphics->get_rect(&rect_wnd);

		auto client_start = view.view_to_client(point_start_.point(), rect_wnd);
		auto client_end = view.view_to_client(point_end_.point(), rect_wnd);
		p_graphics->fill_ellipse(
			client_start,
			radius, radius, color
		);
		p_graphics->fill_ellipse(
			client_end,
			radius, radius, color
		);
	}

	void GraphCurve::reverse() noexcept {
		auto point_tmp = point_start_.point();
		point_start_ = mkaul::Point{ 1., 1. } - point_end_.point();
		point_end_ = mkaul::Point{ 1., 1. } - point_tmp;
	}

	bool GraphCurve::is_hovered(const mkaul::Point<double>& point, float box_width, const GraphView& view) const noexcept {
		return is_point_hovered(point, box_width, view) or is_handle_hovered(point, box_width, view);
	}

	bool GraphCurve::is_point_hovered(const mkaul::Point<double>& point, float box_width, const GraphView& view) const noexcept {
		return point_start_.is_hovered(point, box_width, view) or point_end_.is_hovered(point, box_width, view);
	}

	bool GraphCurve::point_move(ActivePoint active_point, const mkaul::Point<double>& point) noexcept {
		switch (active_point) {
		case ActivePoint::Start:
			point_start_.move(mkaul::Point{ std::min(point.x, point_end_.x()), point.y });
			break;

		case ActivePoint::End:
			point_end_.move(mkaul::Point{ std::max(point.x, point_start_.x()), point.y });
			break;

		default:
			return false;
		}
		return true;
	}

	void GraphCurve::point_end_control() noexcept {
		point_start_.end_control();
		point_end_.end_control();
	}
}