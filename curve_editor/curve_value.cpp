#include "curve.hpp"
#include "curve_value.hpp"


namespace cved {
	ValueCurve::ValueCurve(
		const mkaul::Point<double>& pt_start,
		const mkaul::Point<double>& pt_end
	)
	{
		
	}

	double ValueCurve::curve_function(double progress, double start, double end) const noexcept
	{
		return 0.;
	}

	void ValueCurve::clear() noexcept
	{
	}

	void ValueCurve::create_data(std::vector<byte>& data) const noexcept
	{
	}

	bool ValueCurve::load_data(const byte* data, size_t size) noexcept
	{
		return false;
	}

	void ValueCurve::draw_handle(
		mkaul::graphics::Graphics* p_graphics,
		const View& view,
		float thickness,
		float root_radius,
		float tip_radius,
		float tip_thickness,
		bool cutoff_line,
		const mkaul::ColorF& color
	) const noexcept
	{
	}

	bool ValueCurve::add_curve(const mkaul::Point<double>& pt, const GraphView& view) noexcept
	{
		return false;
	}

	bool ValueCurve::delete_curve(const mkaul::Point<double>& pt, const GraphView& view) noexcept
	{
		return false;
	}

	bool ValueCurve::replace_curve(size_t idx, CurveSegmentType segment_type) noexcept
	{
		return false;
	}

	bool ValueCurve::is_pt_hovered(const mkaul::Point<double>& pt, const GraphView& view) const noexcept
	{
		return false;
	}

	bool ValueCurve::is_handle_hovered(const mkaul::Point<double>& pt, const GraphView& view) const noexcept
	{
		return false;
	}

	bool ValueCurve::handle_check_hover(
		const mkaul::Point<double>& pt,
		const GraphView& view
	) noexcept
	{
		return false;
	}

	bool ValueCurve::handle_update(
		const mkaul::Point<double>& pt,
		const GraphView& view
	) noexcept
	{
		return false;
	}

	void ValueCurve::handle_end_control() noexcept
	{
	}

	ValueCurve::ActivePoint ValueCurve::pt_check_hover(const mkaul::Point<double>& pt, const GraphView& view) noexcept
	{
		return ActivePoint{};
	}

	ValueCurve::ActivePoint ValueCurve::pt_update(const mkaul::Point<double>& pt, const GraphView& view) noexcept
	{
		return ActivePoint{};
	}

	void ValueCurve::pt_end_move() noexcept
	{
	}

	void ValueCurve::pt_end_control() noexcept
	{
	}
}