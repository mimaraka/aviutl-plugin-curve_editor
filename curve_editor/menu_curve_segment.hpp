#pragma once

#include "curve_graph.hpp"
#include "menu.hpp"
#include "my_webview2.hpp"



namespace cved {
	class CurveSegmentMenu : public Menu {
		HMENU submenu_segment_type_;
		void update_state(uint32_t curve_id) noexcept;
		bool callback(uint32_t curve_id, const MyWebView2& webview, uint16_t id) noexcept;

	public:
		CurveSegmentMenu(HINSTANCE hinst);

		HMENU get_handle(uint32_t curve_id) noexcept;
		int show(
			uint32_t curve_id,
			const MyWebView2& webview,
			HWND hwnd,
			UINT flags = NULL,
			const mkaul::Point<LONG>* p_custom_pt_screen = nullptr
		) noexcept;
	};
}