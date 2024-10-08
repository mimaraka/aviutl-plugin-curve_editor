#pragma once

#include "curve_graph.hpp"
#include "enum.hpp"



namespace cved {
	// カーブ(数値指定)
	class ValueCurve : public GraphCurve {
		std::vector<std::unique_ptr<GraphCurve>> curve_segments_;

	public:
		ValueCurve(
			const mkaul::Point<double>& anchor_start = mkaul::Point{ 0., 0. },
			const mkaul::Point<double>& anchor_end = mkaul::Point{ 1., 1. }
		) noexcept;

		ValueCurve(const ValueCurve& curve) noexcept;

		std::string get_type() const noexcept override { return "value"; }

		double curve_function(double progress, double start, double end) const noexcept override;
		void clear() noexcept override;
		bool is_default() const noexcept override;

		bool add_curve(const mkaul::Point<double>& pt, double scale_x) noexcept;
		bool delete_curve(GraphCurve* p_segment) noexcept;
		bool replace_curve(size_t idx, CurveSegmentType segment_type) noexcept;
	};
}