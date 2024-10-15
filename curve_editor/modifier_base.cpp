#include "modifier_base.hpp"
#include <strconv2.h>



namespace cved {
	nlohmann::json Modifier::create_json() const noexcept {
		nlohmann::json data;
		data["name"] = ::sjis_to_utf8(name_);
		data["type"] = get_type();
		data["enabled"] = enabled_;
		return data;
	}
} // namespace cved