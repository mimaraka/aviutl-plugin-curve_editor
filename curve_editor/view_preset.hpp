#pragma once

#include "view.hpp"



namespace cved {
	class PresetView : public View {
	public:
		double client_to_view_x(const float x_client, const mkaul::WindowRectangle& rect_wnd) const noexcept override;
		double client_to_view_y(const float y_client, const mkaul::WindowRectangle& rect_wnd) const noexcept override;
		mkaul::Point<double> client_to_view(const mkaul::Point<float>& point_client, const mkaul::WindowRectangle& rect_wnd) const noexcept override;

		float view_to_client_x(const double x_view, const mkaul::WindowRectangle& rect_wnd) const noexcept override;
		float view_to_client_y(const double y_view, const mkaul::WindowRectangle& rect_wnd) const noexcept override;
		mkaul::Point<float> view_to_client(const mkaul::Point<double>& point_view, const mkaul::WindowRectangle& rect_wnd) const noexcept override;
	};
}