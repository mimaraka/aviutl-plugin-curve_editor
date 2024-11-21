#pragma once

#include "exedit_hook.hpp"
#include <d2d1.h>
#include <mkaul/graphics.hpp>
#include <mkaul/rectangle.hpp>



namespace curve_editor {
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
		void highlight(ID2D1DCRenderTarget* p_render_target) const noexcept;
		void unhighlight() const noexcept;
	};
} // namespace curve_editor