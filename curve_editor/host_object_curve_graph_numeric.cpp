#include "curve_graph_numeric.hpp"
#include "host_object_curve_graph_numeric.hpp"



namespace cved {
	int32_t NumericGraphCurveHostObject::encode(uint32_t id) {
		auto curve = global::id_manager.get_curve<NumericGraphCurve>(id);
		if (!curve) {
			return 0;
		}
		return curve->encode();
	}

	bool NumericGraphCurveHostObject::decode(uint32_t id, int32_t code) {
		auto curve = global::id_manager.get_curve<NumericGraphCurve>(id);
		if (!curve) {
			return false;
		}
		return curve->decode(code);
	}
}