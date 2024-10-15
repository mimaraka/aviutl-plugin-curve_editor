#pragma once

#include "menu.hpp"
#include "my_webview2.hpp"



namespace cved {
	class GraphMenu : public Menu {
		HMENU submenu_apply_mode_;
		void update_state(EditMode mode) noexcept;
		bool callback(EditMode mode, uint32_t curve_id, const MyWebView2& webview, HWND hwnd, uint16_t id) noexcept;

	public:
		GraphMenu(HINSTANCE hinst);

		HMENU get_handle(EditMode mode) noexcept;
		int show(
			EditMode mode,
			uint32_t curve_id,
			const MyWebView2& webview,
			HWND hwnd,
			UINT flags = NULL,
			const mkaul::Point<LONG>* p_custom_pt_screen = nullptr
		) noexcept;
	};
}