#include "modifier_base.hpp"
#include <strconv2.h>



namespace curve_editor {
	nlohmann::json Modifier::create_json() const noexcept {
		nlohmann::json data;
		data["name"] = ::wide_to_utf8(name_);
		data["type"] = get_type();
		data["enabled"] = enabled_;
		return data;
	}
} // namespace curve_editor