#pragma once

#include "curve_graph.hpp"



namespace cved {
	// カーブ(直線)
	class LinearCurve : public GraphCurve {
	public:
		//using GraphCurve::GraphCurve;
		LinearCurve(
			const mkaul::Point<double>& pt_start = mkaul::Point{ 0., 0. },
			const mkaul::Point<double>& pt_end = mkaul::Point{ 1., 1. },
			bool pt_fixed = false,
			GraphCurve* prev = nullptr,
			GraphCurve* next = nullptr
		) noexcept :
			GraphCurve{ pt_start, pt_end, pt_fixed, prev, next }
		{}

		// カーブの値を取得
		double curve_function(double progress, double start, double end) const noexcept override;
		// カーブを初期化
		void clear() noexcept override {}

		// ハンドルを描画
		void draw_handle(mkaul::graphics::Graphics*, const View&, float, float, float, float, bool, const mkaul::ColorF&) const noexcept override {}
		
		bool handle_check_hover(
			const mkaul::Point<double>&,
			const GraphView&
		) noexcept override { return false; }
		bool handle_update(
			const mkaul::Point<double>&,
			const GraphView&
		) noexcept override { return false; }
		void handle_end_control() noexcept override {}
		bool is_handle_hovered(const mkaul::Point<double>&, const GraphView&) const noexcept override { return false; }

		ActivePoint pt_check_hover(const mkaul::Point<double>& pt, const GraphView& view) noexcept override;
		bool pt_begin_move(ActivePoint) noexcept override { return true; }
		ActivePoint pt_update(const mkaul::Point<double>& pt, const GraphView& view) noexcept override;
		void pt_end_control() noexcept override;

		template <class Archive>
		void save(Archive& archive, const std::uint32_t) const {
			archive(
				cereal::base_class<GraphCurve>(this)
			);
		}

		template <class Archive>
		void load(Archive& archive, const std::uint32_t) {
			archive(
				cereal::base_class<GraphCurve>(this)
			);
		}
	};
}

CEREAL_CLASS_VERSION(cved::LinearCurve, 0)
CEREAL_REGISTER_TYPE(cved::LinearCurve)
CEREAL_REGISTER_POLYMORPHIC_RELATION(cved::GraphCurve, cved::LinearCurve)