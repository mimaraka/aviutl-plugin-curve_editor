#include "trackbar_button.hpp"



namespace cved {
	bool TrackbarButton::init(HWND hwnd_obj_dialog, int id) noexcept {
		hwnd_ = ::GetDlgItem(hwnd_obj_dialog, 4000 + id);
		if (hwnd_) {
			hwnd_obj_dialog_ = hwnd_obj_dialog;
			track_idx_ = id;
			return true;
		}
		else return false;
	}

	bool TrackbarButton::get_screen_rect(mkaul::WindowRectangle* p_rect_wnd) const noexcept {
		RECT tmp;
		if (::GetWindowRect(hwnd_, &tmp)) {
			*p_rect_wnd = tmp;
			return true;
		}
		else return false;
		
	}

	bool TrackbarButton::is_hovered() const noexcept {
		RECT rect_tmp;
		if (::GetWindowRect(hwnd_, &rect_tmp)) {
			POINT pt_tmp;
			if (::GetCursorPos(&pt_tmp)) {
				return ::PtInRect(&rect_tmp, pt_tmp);
			}
		}
		return false;
	}
}