#pragma once

#include <mkaul/include/button_icon.hpp>



namespace cved {
	class IconButtonWithBadge : public mkaul::ui::IconButton {

	public:
		void add_badge();
		void remove_badge();
	};
}