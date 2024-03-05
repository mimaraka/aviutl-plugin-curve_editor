#include "curve.hpp"



namespace cved {
	double Curve::get_velocity(double progress, double start, double end) const noexcept {
		constexpr double DELTA = 0.000001;
		constexpr double TMP = 1. / DELTA;
		double prog_before, prog_after;
		if (progress <= DELTA) {
			prog_before = progress;
			prog_after = progress + DELTA;
		}
		else {
			prog_before = progress - DELTA;
			prog_after = progress;
		}
		return (get_value(prog_after, start, end) - get_value(prog_before, start, end)) * TMP;
	}
}