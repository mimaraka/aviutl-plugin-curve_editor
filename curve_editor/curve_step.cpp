#include "curve_step.hpp"
#include "curve_data.hpp"
#include "enum.hpp"



namespace cved {
	double StepCurve::get_value(double progress, double start, double end) const noexcept {
		return 0.f;
	}

	void StepCurve::clear() noexcept {

	}

	void StepCurve::reverse() noexcept {

	}

	void StepCurve::create_data(std::vector<byte>& data) const noexcept {
		StepCurveData data_step{
			.data_graph = GraphCurveData{
				.start_x = point_start_.x(),
				.start_y = point_start_.y(),
				.end_x = point_end_.x(),
				.end_y = point_end_.y()
			}
		};
		auto bytes_step = reinterpret_cast<byte*>(&data_step);
		size_t n = sizeof(ElasticCurveData) / sizeof(byte);
		data = std::vector<byte>{ bytes_step, bytes_step + n };
		data.insert(data.begin(), (byte)CurveSegmentType::Step);
	}

	bool StepCurve::load_data(const byte* data, size_t size) noexcept {
		if (size < sizeof(StepCurveData) / sizeof(byte)) return false;
		auto p_curve_data = reinterpret_cast<const StepCurveData*>(data);
		// カーブの整合性チェック
		if (
			!mkaul::real_in_range(p_curve_data->data_graph.start_x, 0., 1., true)
			or !mkaul::real_in_range(p_curve_data->data_graph.end_x, 0., 1., true)
			or p_curve_data->data_graph.end_x < p_curve_data->data_graph.start_x
			) {
			return false;
		}
		point_start_.move(mkaul::Point{ p_curve_data->data_graph.start_x, p_curve_data->data_graph.start_y });
		point_end_.move(mkaul::Point{ p_curve_data->data_graph.end_x, p_curve_data->data_graph.end_y });
		return true;
	}

	int32_t StepCurve::encode() const noexcept {
		return 0;
	}

	bool StepCurve::decode(int32_t code) noexcept {
		return true;
	}

	void StepCurve::draw_handle(
		mkaul::graphics::Graphics* p_graphics,
		const View& view,
		float thickness,
		float root_radius,
		float tip_radius,
		float tip_thickness,
		bool cutoff_line,
		const mkaul::ColorF& color
	) const noexcept {

	}

	bool StepCurve::is_handle_hovered(const mkaul::Point<double>& point, float box_width, const GraphView& view) const noexcept {
		return false;
	}

	bool StepCurve::handle_check_hover(
		const mkaul::Point<double>& point,
		float box_width,
		const GraphView& view
	) noexcept {
		return false;
	}

	bool StepCurve::handle_update(
		const mkaul::Point<double>& point,
		const GraphView& view
	) noexcept {
		return false;
	}

	void StepCurve::handle_end_control() noexcept {

	}

	StepCurve::ActivePoint StepCurve::point_check_hover(const mkaul::Point<double>& point, float box_width, const GraphView& view) noexcept {
		return ActivePoint::Null;
	}

	bool StepCurve::point_begin_move(ActivePoint active_point) noexcept {
		return true;
	}

	StepCurve::ActivePoint StepCurve::point_update(const mkaul::Point<double>& point, const GraphView& view) noexcept {
		return ActivePoint::Null;
	}

	bool StepCurve::point_move(ActivePoint active_point, const mkaul::Point<double>& point) noexcept {
		return true;
	}

	void StepCurve::point_end_move() noexcept {

	}

	void StepCurve::point_end_control() noexcept {

	}
}