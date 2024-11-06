#pragma once

#include "menu_item.hpp"
#include <mkaul/point.hpp>



namespace cved {
	class ContextMenu {
		HMENU menu_;
		std::vector<MenuItem> items_;
		void init() noexcept;

	public:
		ContextMenu(std::vector<MenuItem> vec) : items_{ vec } { init(); }
		ContextMenu(std::initializer_list<MenuItem> list) : items_{ list } { init(); }

		~ContextMenu() {
			::DestroyMenu(menu_);
		}

		bool show(
			HWND hwnd,
			UINT flags = NULL,
			const mkaul::Point<LONG>* p_custom_pt_screen = nullptr
		) noexcept;
	};
} // namespace cved