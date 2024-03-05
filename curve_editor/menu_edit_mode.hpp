#pragma once

#include "menu.hpp"


namespace cved {
	class EditModeMenu : public Menu {
		void update_state() noexcept override;

	public:
		EditModeMenu();

		bool callback(WPARAM wparam, LPARAM lparam) noexcept override;
	};
}