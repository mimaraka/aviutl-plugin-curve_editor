#include "config.hpp"
#include "curve.hpp"
#include "trackbar_button.hpp"



namespace curve_editor {
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
		constexpr int MARGIN = 1;
		mkaul::WindowRectangle rect_button;
		if (rect_button.from_window_rect(hwnd_)) {
			POINT pt_screen;
			if (::GetCursorPos(&pt_screen)) {
				rect_button.set_margin(-MARGIN, -MARGIN, -MARGIN, -MARGIN);
				return rect_button.pt_in_rect(pt_screen);
			}
		}
		return false;
	}

	void TrackbarButton::highlight(ID2D1DCRenderTarget* p_render_target, uint32_t curve_id) const noexcept {
		constexpr float ROUND_RADIUS = 1.5f;
		constexpr mkaul::ColorF COLOR_NORMAL{ 45, 140, 235 };
		constexpr mkaul::ColorF COLOR_IGNORE_MID_POINT = { 243, 123, 52 };
		constexpr mkaul::ColorF COLOR_INTERPOLATE = { 90, 182, 82 };

		if (p_render_target) {
			mkaul::WindowRectangle rect_button;
			get_screen_rect(&rect_button);
			rect_button.screen_to_client(hwnd_obj_dialog_);
			rect_button.set_margin(-2, -2, -2, -2);
			mkaul::ColorF color;
			static ID2D1SolidColorBrush* p_brush = nullptr;
			RECT rect_wnd;

			auto curve = global::id_manager.get_curve<Curve>(curve_id);
			if (!curve) return;

			// ApplyModeに応じた色の設定
			switch (global::config.get_apply_mode(curve->get_type())) {
			case ApplyMode::Normal:
				color = COLOR_NORMAL;
				break;

			case ApplyMode::IgnoreMidPoint:
				color = COLOR_IGNORE_MID_POINT;
				break;

			case ApplyMode::Interpolate:
				color = COLOR_INTERPOLATE;
				break;

			default:
				return;
			}

			::GetClientRect(hwnd_obj_dialog_, &rect_wnd);
			auto hdc = ::GetDC(hwnd_obj_dialog_);
			p_render_target->BindDC(hdc, &rect_wnd);
			p_render_target->BeginDraw();
			p_render_target->SetTransform(D2D1::Matrix3x2F::Identity());
			if (!p_brush) {
				auto hr = p_render_target->CreateSolidColorBrush(D2D1::ColorF(0, 0, 0), &p_brush);
				if (FAILED(hr)) {
					return;
				}
			}
			p_brush->SetColor(color.d2d1_colorf());
			D2D1_ROUNDED_RECT d2d_rect = D2D1::RoundedRect(
				D2D1::RectF(
					(float)(rect_button.left),
					(float)(rect_button.top),
					(float)(rect_button.right),
					(float)(rect_button.bottom)),
				ROUND_RADIUS, ROUND_RADIUS
			);
			p_brush->SetOpacity(0.3f);
			p_render_target->FillRoundedRectangle(d2d_rect, p_brush);

			p_brush->SetOpacity(0.7f);
			p_render_target->DrawRoundedRectangle(d2d_rect, p_brush, 2.f);
			p_render_target->EndDraw();
		}
	}

	void TrackbarButton::unhighlight() const noexcept {
		mkaul::WindowRectangle rect_button;
		get_screen_rect(&rect_button);
		rect_button.screen_to_client(hwnd_obj_dialog_);
		rect_button.set_margin(-3, -3, -3, -3);
		RECT rect = rect_button.get_rect();

		::InvalidateRect(hwnd_obj_dialog_, &rect, FALSE);
		::UpdateWindow(hwnd_obj_dialog_);
	}
} // namespace curve_editor