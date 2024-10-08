#define NOMINMAX
#include "view_graph.hpp"
#include "config.hpp"



namespace cved {
	void GraphView::begin_move(const mkaul::Point<float>& pt) noexcept {
		moving_ = true;
		pt_buffer_ = pt;
		center_buffer_ = center_;
	}

	bool GraphView::move(const mkaul::Point<float>& pt, const mkaul::WindowRectangle&) noexcept {
		if (moving_) {
			auto pt_offset = pt - pt_buffer_;
			center_ = center_buffer_ - mkaul::Point{ pt_offset.x / scale_x_, -pt_offset.y / scale_y_ };
			return true;
		}
		else return false;
	}

	void GraphView::move(double scroll_amount, bool vertical) noexcept {
		constexpr double GRAPH_WHEEL_COEF_POS = 0.2;
		if (vertical) {
			center_.y += scroll_amount * GRAPH_WHEEL_COEF_POS / scale_y_;
		}
		else {
			center_.x -= scroll_amount * GRAPH_WHEEL_COEF_POS / scale_x_;
		}

	}

	void GraphView::begin_scale(const mkaul::Point<float>& pt) noexcept {
		scaling_ = true;
		pt_buffer_ = pt;
		scale_x_buffer_ = scale_x_;
		scale_y_buffer_ = scale_y_;
	}

	bool GraphView::scale(const mkaul::Point<float>& pt) noexcept {
		if (scaling_) {
			double coef_x, coef_y;

			coef_x = mkaul::clamp(std::pow(GRAPH_SCALE_BASE, pt.x - pt_buffer_.x),
				SCALE_MIN / scale_x_buffer_, SCALE_MAX / scale_x_buffer_);
			coef_y = mkaul::clamp(std::pow(GRAPH_SCALE_BASE, pt_buffer_.y - pt.y),
				SCALE_MIN / scale_y_buffer_, SCALE_MAX / scale_y_buffer_);
			if (::GetAsyncKeyState(VK_SHIFT) & 0x8000) {
				coef_x = coef_y = std::max(coef_x, coef_y);
				scale_x_buffer_ = scale_y_buffer_ = std::max(scale_x_buffer_, scale_y_buffer_);
			}
			scale_x_ = scale_x_buffer_ * coef_x;
			scale_y_ = scale_y_buffer_ * coef_y;
			return true;
		}
		else return false;
	}

	void GraphView::scale(double scroll_amount) noexcept {
		constexpr double GRAPH_WHEEL_COEF_SCALE = 0.1;
		double coef = std::pow(GRAPH_SCALE_BASE, scroll_amount * GRAPH_WHEEL_COEF_SCALE);
		if (std::max(scale_x_, scale_y_) > SCALE_MAX / coef) {
			coef = SCALE_MAX / std::max(scale_x_, scale_y_);
		}
		else if (std::min(scale_x_, scale_y_) < SCALE_MIN / coef) {
			coef = SCALE_MIN / std::min(scale_x_, scale_y_);
		}
		scale_x_ *= coef;
		scale_y_ *= coef;
	}

	void GraphView::fit(const mkaul::WindowRectangle& rect_wnd) noexcept {
		constexpr float GRAPH_PADDING = 20.;

		center_ = mkaul::Point{ 0.5, 0.5 };
		double tmp_scale_x = std::max(static_cast<double>(rect_wnd.get_width()) - GRAPH_PADDING * 2., 0.);
		double tmp_scale_y = std::max(static_cast<double>(rect_wnd.get_height()) - GRAPH_PADDING * 2., 0.);
		if (tmp_scale_x < tmp_scale_y) {
			scale_x_ = scale_y_ = tmp_scale_x;
		}
		else {
			scale_x_ = tmp_scale_x;
			scale_y_ = tmp_scale_y;
		}
	}

	void GraphView::draw_grid(mkaul::graphics::Graphics* p_graphics, const mkaul::WindowRectangle& rect_wnd) const noexcept {
		constexpr float BRIGHTNESS_GRID = 0.1f;
		constexpr double MAX_DELTA = 60.;
		constexpr float GRID_WEIGHT_BOLD = 1.f;
		constexpr float GRID_WEIGHT_THIN = 0.5f;

		mkaul::ColorF color_grid = global::config.get_theme().bg_graph;
		color_grid.change_brightness(BRIGHTNESS_GRID);
		// 
		int nx = (int)std::ceil(std::log2(scale_x_ / MAX_DELTA));
		int ny = (int)std::ceil(std::log2(scale_y_ / MAX_DELTA));

		double dx_view = std::pow(0.5, nx);
		double dy_view = std::pow(0.5, ny);

		// 縦方向
		for (
			int i = (int)std::ceil(client_to_view_x(0.f, rect_wnd) / dx_view);
			view_to_client_x(i * dx_view, rect_wnd) <= rect_wnd.right;
			i++
			) {
			float x = view_to_client_x(i * dx_view, rect_wnd);
			p_graphics->draw_line(
				mkaul::Point{ x, 0.f },
				mkaul::Point{ x, (float)rect_wnd.bottom },
				color_grid,
				mkaul::graphics::Stroke{
					i % 2 ? GRID_WEIGHT_THIN : GRID_WEIGHT_BOLD,
					i % 2 ? mkaul::graphics::Stroke::DashStyle::Dash : mkaul::graphics::Stroke::DashStyle::Solid
				}
			);
		}

		// 横方向
		for (
			int i = (int)std::ceil(client_to_view_y(0.f, rect_wnd) / dy_view);
			view_to_client_y(i * dy_view, rect_wnd) <= rect_wnd.bottom;
			i--
			) {
			float y = view_to_client_y(i * dy_view, rect_wnd);
			p_graphics->draw_line(
				mkaul::Point{ 0.f, y },
				mkaul::Point{ (float)rect_wnd.right, y },
				color_grid,
				mkaul::graphics::Stroke{
					i % 2 ? GRID_WEIGHT_THIN : GRID_WEIGHT_BOLD,
					i % 2 ? mkaul::graphics::Stroke::DashStyle::Dash : mkaul::graphics::Stroke::DashStyle::Solid
				}
			);
		}
	}

	double GraphView::client_to_view_x(const float x_client, const mkaul::WindowRectangle& rect_wnd) const noexcept {
		auto center_client_x = (rect_wnd.left + rect_wnd.right) * 0.5f;
		auto offset_client_x = x_client - center_client_x;
		auto offset_view_x = static_cast<double>(offset_client_x / scale_x_);
		return center_.x + offset_view_x;
	}

	double GraphView::client_to_view_y(const float y_client, const mkaul::WindowRectangle& rect_wnd) const noexcept {
		auto center_client_y = (rect_wnd.top + rect_wnd.bottom) * 0.5f;
		auto offset_client_y = y_client - center_client_y;
		auto offset_view_y = static_cast<double>(-offset_client_y / scale_y_);
		return center_.y + offset_view_y;
	}

	mkaul::Point<double> GraphView::client_to_view(const mkaul::Point<float>& pt_client, const mkaul::WindowRectangle& rect_wnd) const noexcept {
		return mkaul::Point{
			client_to_view_x(pt_client.x, rect_wnd),
			client_to_view_y(pt_client.y, rect_wnd)
		};
	}

	float GraphView::view_to_client_x(const double x_view, const mkaul::WindowRectangle& rect_wnd) const noexcept {
		auto offset_view_x = x_view - center_.x;
		auto offset_client_x = static_cast<float>(offset_view_x * scale_x_);
		auto center_client_x = (rect_wnd.left + rect_wnd.right) * 0.5f;
		return center_client_x + offset_client_x;
	}

	float GraphView::view_to_client_y(const double y_view, const mkaul::WindowRectangle& rect_wnd) const noexcept {
		auto offset_view_y = y_view - center_.y;
		auto offset_client_y = static_cast<float>(-offset_view_y * scale_y_);
		auto center_client_y = (rect_wnd.top + rect_wnd.bottom) * 0.5f;
		return center_client_y + offset_client_y;
	}

	mkaul::Point<float> GraphView::view_to_client(const mkaul::Point<double>& pt_view, const mkaul::WindowRectangle& rect_wnd) const noexcept {
		return mkaul::Point{
			view_to_client_x(pt_view.x, rect_wnd),
			view_to_client_y(pt_view.y, rect_wnd)
		};
	}
}