#pragma once

#include "menu.hpp"



namespace cved {
	class MoreMenu : public Menu {
		void update_state() noexcept override;
		bool callback(uint16_t id) noexcept override;

	public:
		MoreMenu(HINSTANCE hinst);
	};
}