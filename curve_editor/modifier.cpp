#include "modifier.hpp"



namespace cved {
	CurveFunction Modifier::apply(const CurveFunction& function) const noexcept {
		if (!enabled_) {
			return function;
		}
		return convert(function);
	}
}