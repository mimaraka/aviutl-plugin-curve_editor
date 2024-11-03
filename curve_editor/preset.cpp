#include "config.hpp"
#include "constants.hpp"
#include "curve_normal.hpp"
#include "curve_value.hpp"
#include "curve_bezier.hpp"
#include "curve_elastic.hpp"
#include "curve_bounce.hpp"
#include "curve_linear.hpp"
#include "curve_script.hpp"
#include "global.hpp"
#include "preset.hpp"
#include <strconv2.h>



namespace cved {
	nlohmann::json Preset::create_json() const noexcept {
		nlohmann::json data;
		data["name"] = ::sjis_to_utf8(name_);
		data["curve"] = p_curve_->create_json();
		return data;
	}

	bool Preset::load_json(const nlohmann::json& data) noexcept {
		try {
			name_ = ::utf8_to_sjis(data.at("name").get<std::string>());
			auto curve_type = data.at("curve").at("type").get<std::string>();
			if (curve_type == global::CURVE_NAME_NORMAL) {
				p_curve_ = std::make_unique<NormalCurve>();
			}
			else if (curve_type == global::CURVE_NAME_VALUE) {
				p_curve_ = std::make_unique<ValueCurve>();
			}
			else if (curve_type == global::CURVE_NAME_BEZIER) {
				p_curve_ = std::make_unique<BezierCurve>();
			}
			else if (curve_type == global::CURVE_NAME_ELASTIC) {
				p_curve_ = std::make_unique<ElasticCurve>();
			}
			else if (curve_type == global::CURVE_NAME_BOUNCE) {
				p_curve_ = std::make_unique<BounceCurve>();
			}
			else if (curve_type == global::CURVE_NAME_LINEAR) {
				p_curve_ = std::make_unique<LinearCurve>();
			}
			else if (curve_type == global::CURVE_NAME_SCRIPT) {
				p_curve_ = std::make_unique<ScriptCurve>();
			}		
			if (!p_curve_ or !p_curve_->load_json(data.at("curve"))) {
				return false;
			}
		}
		catch (const nlohmann::json::exception&) {
			return false;
		}
		return true;
	}
} // namespace cved