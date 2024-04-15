#pragma once

#include "menu.hpp"
#include "curve_bezier.hpp"
#include "handle_bezier.hpp"



namespace cved {
	class BezierHandleMenu : public Menu {
		void update_state(const BezierHandle* p_handle) noexcept;
		bool callback(BezierHandle* p_handle, const GraphView& view, uint16_t id) noexcept;

	public:
		BezierHandleMenu(HINSTANCE hinst);
		HMENU get_handle(const BezierHandle* p_handle) noexcept;
		int show(
			BezierHandle* p_handle,
			const GraphView& view,
			HWND hwnd,
			UINT flags = NULL,
			const mkaul::Point<LONG>* p_custom_pt_screen = nullptr
		) noexcept;
	};
}