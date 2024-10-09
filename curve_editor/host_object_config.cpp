#include "constants.hpp"
#include "host_object_config.hpp"
#include <mkaul/version.hpp>
#include <strconv2.h>



namespace cved {
	std::wstring ConfigHostObject::get_curve_color() {
		mkaul::ColorI8 color_i8 = global::config.get_curve_color();
		return std::format(L"#{:02x}{:02x}{:02x}", color_i8.get_r(), color_i8.get_g(), color_i8.get_b());
	}

	bool ConfigHostObject::is_latest_version(std::wstring latest_version_str) {
		mkaul::Version latest;
		if (!latest.from_str(::wide_to_sjis(latest_version_str))) {
			return false;
		}
		return latest <= global::PLUGIN_VERSION;
	}
}