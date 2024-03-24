#pragma once

#include "menu.hpp"



namespace cved {
	class GraphMenu : public Menu {
		void update_state() noexcept override;

	public:
		GraphMenu(HINSTANCE hinst);

		bool callback(WPARAM wparam, LPARAM lparam) noexcept override;
	};
}