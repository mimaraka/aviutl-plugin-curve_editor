#pragma once

#include <mkaul/color.hpp>


namespace cved {
	struct Theme {
		mkaul::ColorF bg;
		mkaul::ColorF bg_graph;
		mkaul::ColorF bg_editbox;
		mkaul::ColorF separator;
		mkaul::ColorF curve_trace;
		mkaul::ColorF curve_velocity;
		mkaul::ColorF curve_preset;
		mkaul::ColorF handle;
		mkaul::ColorF handle_preset;
		mkaul::ColorF button_selected;
		mkaul::ColorF button_unselected;
		mkaul::ColorF button_label;
		mkaul::ColorF button_label_selected;
		mkaul::ColorF preset_label;
		mkaul::ColorF editbox_text;
	};
}