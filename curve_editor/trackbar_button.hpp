#pragma once

#include "exedit_hook.hpp"
#include <mkaul/rectangle.hpp>
#include <mkaul/graphics.hpp>



namespace cved {
	class TrackbarButton {
		int track_idx_ = 0;
		HWND hwnd_ = NULL;
		HWND hwnd_obj_dialog_ = NULL;

	public:
		bool init(HWND hwnd_obj_dialog, int id) noexcept;
		bool get_screen_rect(mkaul::WindowRectangle* p_rect_wnd) const noexcept;
		auto track_idx() const noexcept { return track_idx_; }
		bool is_hovered() const noexcept;
		bool is_visible() const noexcept { return ::IsWindowVisible(hwnd_); }
		void highlight(mkaul::graphics::Graphics* p_graphics) const noexcept;
	};
}