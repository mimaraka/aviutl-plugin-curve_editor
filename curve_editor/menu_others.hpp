#pragma once

#include "menu.hpp"



namespace cved {
	class OthersMenu : public Menu {
		void update_state() noexcept override;
		bool callback(uint16_t id) noexcept override;

	public:
		OthersMenu(HINSTANCE hinst);
	};
}