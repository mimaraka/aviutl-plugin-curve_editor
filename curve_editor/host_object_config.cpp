#include "constants.hpp"
#include "host_object_config.hpp"
#include <mkaul/version.hpp>
#include <strconv2.h>



namespace curve_editor {
	std::wstring ConfigHostObject::get_curve_color() {
		mkaul::ColorI8 color_i8 = global::config.get_curve_color();
		return std::format(L"#{:02x}{:02x}{:02x}", color_i8.get_r(), color_i8.get_g(), color_i8.get_b());
	}
} // namespace curve_editor