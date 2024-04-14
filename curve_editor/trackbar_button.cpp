#include "trackbar_button.hpp"
#include "config.hpp"



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

	void TrackbarButton::highlight(mkaul::graphics::Graphics* p_graphics) const noexcept {
		constexpr float ROUND_RADIUS = 1.5f;
		constexpr mkaul::ColorF COLOR_NORMAL{ 45, 140, 235 };
		constexpr mkaul::ColorF COLOR_IGNORE_MID_POINT = { 243, 123, 52 };

		if (p_graphics) {
			mkaul::WindowRectangle rect_button;
			mkaul::Rectangle<float> rect_button_f;
			get_screen_rect(&rect_button);
			rect_button.screen_to_client(hwnd_obj_dialog_);
			rect_button.set_margin(-2, -2, -2, -2);
			rect_button.convert_to(&rect_button_f);
			mkaul::ColorF color;

			// ApplyModeに応じた色の設定
			switch (global::config.get_apply_mode(global::config.get_edit_mode())) {
			case ApplyMode::Normal:
				color = COLOR_NORMAL;
				break;

			case ApplyMode::IgnoreMidPoint:
				color = COLOR_IGNORE_MID_POINT;
				break;

			default:
				return;
			}

			p_graphics->begin_draw(false);
			color.set_a(0.3f);
			p_graphics->fill_rectangle(
				rect_button_f,
				ROUND_RADIUS, ROUND_RADIUS,
				color
			);
			color.set_a(0.7f);
			p_graphics->draw_rectangle(
				rect_button_f,
				ROUND_RADIUS, ROUND_RADIUS,
				color,
				mkaul::graphics::Stroke{ 2.f }
			);
			p_graphics->end_draw();
		}
	}
}