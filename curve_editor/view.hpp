#pragma once

#include <mkaul/include/point.hpp>
#include <mkaul/include/rectangle.hpp>



namespace cved {
	class View {
	public:
		virtual double client_to_view_x(const float x_client, const mkaul::WindowRectangle& rect_wnd) const noexcept = 0;
		virtual double client_to_view_y(const float y_client, const mkaul::WindowRectangle& rect_wnd) const noexcept = 0;
		virtual mkaul::Point<double> client_to_view(const mkaul::Point<float>& pt_client, const mkaul::WindowRectangle& rect_wnd) const noexcept = 0;
		
		virtual float view_to_client_x(const double x_view, const mkaul::WindowRectangle& rect_wnd) const noexcept = 0;
		virtual float view_to_client_y(const double y_view, const mkaul::WindowRectangle& rect_wnd) const noexcept = 0;
		virtual mkaul::Point<float> view_to_client(const mkaul::Point<double>& pt_view, const mkaul::WindowRectangle& rect_wnd) const noexcept = 0;
	};
}