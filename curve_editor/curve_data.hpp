#pragma once

#include <cstdint>



namespace cved {
	struct GraphCurveData {
		double start_x;
		double start_y;
		double end_x;
		double end_y;
		uint32_t sampling_resolution;
		uint32_t quantization_resolution;
	};

	struct BezierCurveData {
		GraphCurveData data_graph;
		double left_x;
		double left_y;
		double right_x;
		double right_y;
		int32_t reserved[6];
	};

	struct ElasticCurveData {
		GraphCurveData data_graph;
		double amp;
		double freq;
		double decay;
		int32_t reserved[8];
	};

	struct BounceCurveData {
		GraphCurveData data_graph;
		double cor;
		double period;
		int32_t reserved[10];
	};

	struct LinearCurveData {
		GraphCurveData data_graph;
		int32_t reserved[14];
	};

	struct ScriptCurveData {

	};
}