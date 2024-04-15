#include "curve_linear.hpp"
#include "curve_data.hpp"
#include "enum.hpp"



namespace cved {
	double LinearCurve::curve_function(double progress, double start, double end) const noexcept {
		progress = mkaul::clamp((progress - pt_start().x()) / (pt_end().x() - pt_start().x()), 0., 1.);
		return start + (end - start) * (pt_start().y() + (pt_end().y() - pt_start().y()) * progress);
	}

	void LinearCurve::create_data(std::vector<byte>&data) const noexcept {
		LinearCurveData data_linear{
			.data_graph = GraphCurveData{
				.start_x = pt_start().x(),
				.start_y = pt_start().y(),
				.end_x = pt_end().x(),
				.end_y = pt_end().y(),
				.sampling_resolution = get_sampling_resolution(),
				.quantization_resolution = get_quantization_resolution()
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
		pt_start_.move(mkaul::Point{ p_curve_data->data_graph.start_x, p_curve_data->data_graph.start_y });
		pt_end_.move(mkaul::Point{ p_curve_data->data_graph.end_x, p_curve_data->data_graph.end_y });
		set_sampling_resolution(p_curve_data->data_graph.sampling_resolution);
		set_quantization_resolution(p_curve_data->data_graph.quantization_resolution);
		return true;
	}

	LinearCurve::ActivePoint LinearCurve::pt_check_hover(const mkaul::Point<double>& pt, const GraphView& view) noexcept {
		if (pt_start_.check_hover(pt, view)) return ActivePoint::Start;
		else if (pt_end_.check_hover(pt, view)) return ActivePoint::End;
		else return ActivePoint::Null;
	}

	LinearCurve::ActivePoint LinearCurve::pt_update(const mkaul::Point<double>& pt, const GraphView&) noexcept {
		if (pt_start_.update(mkaul::Point{ std::min(pt.x, pt_end().x()), pt.y })) return ActivePoint::Start;
		else if (pt_end_.update(mkaul::Point{ std::max(pt.x, pt_start().x()), pt.y })) return ActivePoint::End;
		else return ActivePoint::Null;
	}

	void LinearCurve::pt_end_control() noexcept {
		pt_start_.end_control();
		pt_end_.end_control();
	}
}