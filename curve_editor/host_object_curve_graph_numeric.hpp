#pragma once

#include "host_object_curve_graph.hpp"



namespace cved {
	class NumericGraphCurveHostObject : public GraphCurveHostObject {
		static int32_t encode(uint32_t id);
		static bool decode(uint32_t id, int32_t code);

	public:
		NumericGraphCurveHostObject() {
			register_member(L"encode", DispatchType::Method, encode);
			register_member(L"decode", DispatchType::Method, decode);
		}
	};
} // namespace cved