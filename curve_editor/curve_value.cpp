#include "curve.hpp"
#include "curve_value.hpp"


namespace cved {
	ValueCurve::ValueCurve(
		const mkaul::Point<double>& anchor_start,
		const mkaul::Point<double>& anchor_end
	) noexcept
	{
		
	}

	ValueCurve::ValueCurve(const ValueCurve& curve) noexcept
	{
	}

	double ValueCurve::curve_function(double progress, double start, double end) const noexcept
	{
		return 0.;
	}

	void ValueCurve::clear() noexcept
	{
	}

	bool ValueCurve::is_default() const noexcept
	{
		return true;
	}

	bool ValueCurve::add_curve(const mkaul::Point<double>& pt, double scale_x) noexcept
	{
		return false;
	}

	bool ValueCurve::delete_curve(GraphCurve* p_segment) noexcept
	{
		return false;
	}

	bool ValueCurve::replace_curve(size_t idx, CurveSegmentType segment_type) noexcept
	{
		return false;
	}
}