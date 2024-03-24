#include "curve_linear.hpp"
#include "curve_data.hpp"
#include "enum.hpp"



namespace cved {
	double LinearCurve::get_value(double progress, double start, double end) const noexcept {
		progress = mkaul::clamp((progress - point_start_.x()) / (point_end_.x() - point_start_.x()), 0., 1.);
		return start + (end - start) * (point_start_.y() + (point_end_.y() - point_start_.y()) * progress);
	}

	void LinearCurve::create_data(std::vector<byte>&data) const noexcept {
		LinearCurveData data_linear{
			.data_graph = GraphCurveData{
				.start_x = point_start_.x(),
				.start_y = point_start_.y(),
				.end_x = point_end_.x(),
				.end_y = point_end_.y()
			}
		};
		auto bytes_linear = reinterpret_cast<byte*>(&data_linear);
		size_t n = sizeof(LinearCurveData) / sizeof(byte);
		data = std::vector<byte>{ bytes_linear, bytes_linear + n };
		data.insert(data.begin(), (byte)CurveSegmentType::Linear);
	}

	bool LinearCurve::load_data(const byte* data, size_t size) noexcept {
		if (size < sizeof(LinearCurveData) / sizeof(byte)) return false;
		auto p_curve_data = reinterpret_cast<const LinearCurveData*>(data);
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

	LinearCurve::ActivePoint LinearCurve::point_check_hover(const mkaul::Point<double>& point, float box_width, const GraphView& view) noexcept {
		if (point_start_.check_hover(point, box_width, view)) return ActivePoint::Start;
		else if (point_end_.check_hover(point, box_width, view)) return ActivePoint::End;
		else return ActivePoint::Null;
	}

	LinearCurve::ActivePoint LinearCurve::point_update(const mkaul::Point<double>& point, const GraphView&) noexcept {
		if (point_start_.update(mkaul::Point{ std::min(point.x, point_end_.x()), point.y })) return ActivePoint::Start;
		else if (point_end_.update(mkaul::Point{ std::max(point.x, point_start_.x()), point.y })) return ActivePoint::End;
		else return ActivePoint::Null;
	}

	void LinearCurve::point_end_control() noexcept {
		point_start_.end_control();
		point_end_.end_control();
	}
}