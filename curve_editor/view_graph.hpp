#pragma once

#include "view.hpp"
#include <mkaul/include/graphics.hpp>



namespace cved {
	class GraphView : public View {
	private:
		static constexpr double GRAPH_SCALE_BASE = 1.01;
		static constexpr double SCALE_MIN = 1.;
		static constexpr double SCALE_MAX = 1.E7;
		// ビューの中心点のグラフ座標
		mkaul::Point<double> center_;
		// ビューのスケール
		double scale_x_;
		double scale_y_;
		bool moving_;
		bool scaling_;
		mkaul::Point<float> point_buffer_;
		mkaul::Point<double> center_buffer_;
		double scale_x_buffer_;
		double scale_y_buffer_;

	public:
		GraphView() :
			center_{ 0.5, 0.5 },
			scale_x_{ 1. },
			scale_y_{ 1. },
			moving_{ false },
			scaling_{ false },
			point_buffer_{},
			center_buffer_{},
			scale_x_buffer_{ 1. },
			scale_y_buffer_{ 1. }
		{}

		const auto& center() const noexcept { return center_; }
		auto scale_x() const noexcept { return scale_x_; }
		auto scale_y() const noexcept { return scale_y_; }

		void begin_move(const mkaul::Point<float>& point) noexcept;
		bool move(const mkaul::Point<float>& point, const mkaul::WindowRectangle& rect_wnd) noexcept;
		void move(double scroll_amount, bool vertical) noexcept;
		void end_move() noexcept { moving_ = false; }

		void begin_scale(const mkaul::Point<float>& point) noexcept;
		bool scale(const mkaul::Point<float>& point) noexcept;
		void scale(double scroll_amount) noexcept;
		void end_scale() noexcept { scaling_ = false; }

		void fit(const mkaul::WindowRectangle& rect_wnd) noexcept;

		void draw_grid(mkaul::graphics::Graphics* p_graphics, const mkaul::WindowRectangle& rect_wnd) const noexcept;

		double client_to_view_x(const float x_client, const mkaul::WindowRectangle& rect_wnd) const noexcept override;
		double client_to_view_y(const float y_client, const mkaul::WindowRectangle& rect_wnd) const noexcept override;
		mkaul::Point<double> client_to_view(const mkaul::Point<float>& point_client, const mkaul::WindowRectangle& rect_wnd) const noexcept override;
		
		float view_to_client_x(const double x_view, const mkaul::WindowRectangle& rect_wnd) const noexcept override;
		float view_to_client_y(const double y_view, const mkaul::WindowRectangle& rect_wnd) const noexcept override;
		mkaul::Point<float> view_to_client(const mkaul::Point<double>& point_view, const mkaul::WindowRectangle& rect_wnd) const noexcept override;
	};
}