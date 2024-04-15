#pragma once

#include <mkaul/include/point.hpp>
#include <Windows.h>



namespace cved {
	class Menu {
	protected:
		HMENU menu_ = NULL;
		virtual void update_state() noexcept {}
		virtual bool callback(uint16_t) noexcept { return false; }

	public:
		HMENU get_handle() noexcept;
		bool show(
			HWND hwnd,
			UINT flags = NULL,
			const mkaul::Point<LONG>* p_custom_pt_screen = nullptr
		) noexcept;
	};
}