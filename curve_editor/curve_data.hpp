#pragma once



namespace cved {
	struct GraphCurveData {
		double start_x;
		double start_y;
		double end_x;
		double end_y;
	};

	struct BezierCurveData {
		GraphCurveData data_graph;
		double left_x;
		double left_y;
		double right_x;
		double right_y;
		int32_t reserved[8];
	};

	struct ElasticCurveData {
		GraphCurveData data_graph;
		double amp;
		double freq;
		double decay;
		int32_t reserved[10];
	};

	struct BounceCurveData {
		GraphCurveData data_graph;
		double cor;
		double period;
		int32_t reserved[12];
	};

	struct LinearCurveData {
		GraphCurveData data_graph;
		int32_t reserved[16];
	};

	struct StepCurveData {
		GraphCurveData data_graph;
		int32_t reserved[16];
	};

	struct ScriptCurveData {

	};
}