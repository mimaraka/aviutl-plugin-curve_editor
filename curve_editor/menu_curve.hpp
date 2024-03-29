#pragma once

#include "menu.hpp"



namespace cved {
	class CurveMenu : public Menu {
		HMENU submenu_segment_type_;
		void update_state(size_t index) noexcept;
		bool callback(size_t index, uint16_t id) noexcept;

	public:
		CurveMenu(HINSTANCE hinst);

		HMENU get_handle(size_t index) noexcept;
		int show(
			size_t index,
			HWND hwnd,
			UINT flags = NULL,
			const mkaul::Point<LONG>* p_custom_point_screen = nullptr
		) noexcept;
	};
}