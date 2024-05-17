#pragma once

#include "menu.hpp"
#include "curve_bezier.hpp"
#include "handle_bezier.hpp"



namespace cved {
	class BezierHandleMenu : public Menu {
		void update_state(size_t idx, BezierHandle::Type handle_type) noexcept;
		bool callback(size_t idx, BezierHandle::Type handle_type, const GraphView& view, uint16_t id) noexcept;

	public:
		BezierHandleMenu(HINSTANCE hinst);
		HMENU get_handle(size_t idx, BezierHandle::Type handle_type) noexcept;
		int show(
			size_t idx,
			BezierHandle::Type handle_type,
			const GraphView& view,
			HWND hwnd,
			UINT flags = NULL,
			const mkaul::Point<LONG>* p_custom_pt_screen = nullptr
		) noexcept;
	};
}