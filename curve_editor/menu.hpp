#pragma once

#include <mkaul/include/point.hpp>
#include <Windows.h>



namespace cved {
	class Menu {
	protected:
		HMENU menu_ = NULL;
		virtual void update_state() noexcept {}

	public:
		HMENU get_handle() noexcept;
		virtual bool callback(WPARAM wparam, LPARAM lparam) noexcept = 0;
		int show(
			HWND hwnd,
			UINT flags = NULL,
			const mkaul::Point<LONG>* p_custom_point_screen = nullptr
		) noexcept;
	};
}