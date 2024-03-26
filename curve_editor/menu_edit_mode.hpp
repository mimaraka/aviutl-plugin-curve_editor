#pragma once

#include "menu.hpp"


namespace cved {
	class EditModeMenu : public Menu {
		void update_state() noexcept override;
		bool callback(uint16_t id) noexcept override;

	public:
		EditModeMenu();
	};
}