#include "curve_linear.hpp"
#include "enum.hpp"



namespace cved {
	double LinearCurve::curve_function(double progress, double start, double end) const noexcept {
		progress = mkaul::clamp((progress - anchor_start().x) / (anchor_end().x - anchor_start().x), 0., 1.);
		return start + (end - start) * (anchor_start().y + (anchor_end().y - anchor_start().y) * progress);
	}
}