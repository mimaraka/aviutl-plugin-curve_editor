#pragma once

#include "menu.hpp"



namespace cved {
	class GraphMenu : public Menu {
		void update_state() noexcept override;
		bool callback(uint16_t id) noexcept override;

	public:
		GraphMenu(HINSTANCE hinst);
	};
}