#pragma once

#include "curve_editor.hpp"
#include "host_object_curve_bezier.hpp"
#include "host_object_curve_bounce.hpp"
#include "host_object_curve_elastic.hpp"
#include "host_object_curve_graph.hpp"
#include "host_object_curve_graph_numeric.hpp"
#include "host_object_curve_linear.hpp"
#include "host_object_curve_normal.hpp"
#include <mkaul/host_object.hpp>



namespace cved {
	class GraphEditorHostObject : public mkaul::wv2::HostObject {
		static std::vector<double> get_curve_value_array(uint32_t id, double start_x, double start_y, double end_x, double end_y, size_t n);
		static std::vector<double> get_curve_velocity_array(uint32_t id, double start_x, double start_y, double end_x, double end_y, size_t n);
		static std::wstring get_curve_type(uint32_t id);

	public:
		GraphEditorHostObject() {
			register_member(L"getCurveValue", DispatchType::Method, +[](EditMode mode, double prog, double start, double end) {
				return global::editor.editor_graph().get_curve(mode)->get_value(prog, start, end);
			});
			register_member(L"getCurveValueArray", DispatchType::Method, get_curve_value_array);
			register_member(L"getCurveVelocity", DispatchType::Method, +[](EditMode mode, double prog, double start, double end) {
				return global::editor.editor_graph().get_curve(mode)->get_velocity(prog, start, end);
			});
			register_member(L"getCurveVelocityArray", DispatchType::Method, get_curve_velocity_array);
			register_member(L"getCurveType", DispatchType::Method, get_curve_type);
			register_member(L"graph", DispatchType::PropertyGet, create_host_object<GraphCurveHostObject>);
			register_member(L"numeric", DispatchType::PropertyGet, create_host_object<NumericGraphCurveHostObject>);
			register_member(L"bezier", DispatchType::PropertyGet, create_host_object<BezierCurveHostObject>);
			register_member(L"elastic", DispatchType::PropertyGet, create_host_object<ElasticCurveHostObject>);
			register_member(L"bounce", DispatchType::PropertyGet, create_host_object<BounceCurveHostObject>);
			register_member(L"linear", DispatchType::PropertyGet, create_host_object<LinearCurveHostObject>);
			register_member(L"normal", DispatchType::PropertyGet, create_host_object<NormalCurveHostObject>);
		}
	};
} // namespace cved