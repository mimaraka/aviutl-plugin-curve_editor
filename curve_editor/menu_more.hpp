#pragma once

#include "menu.hpp"



namespace cved {
	class MoreMenu : public Menu {
		void update_state() noexcept override;

	public:
		MoreMenu(HINSTANCE hinst);

		bool callback(WPARAM wparam, LPARAM lparam) noexcept override;
	};
}