#pragma once

#include "curve_graph_numeric.hpp"
#include "handle_bounce.hpp"



namespace cved {
	// カーブ(バウンス)
	class BounceCurve : public NumericGraphCurve {
		static constexpr double DEFAULT_COR = 0.6;
		static constexpr double DEFAULT_PERIOD = 0.5;
		BounceHandle handle_;
		double cor_;
		double period_;

	public:
		// コンストラクタ
		BounceCurve(
			const mkaul::Point<double>& pt_start = mkaul::Point{ 0., 0. },
			const mkaul::Point<double>& pt_end = mkaul::Point{ 1., 1. },
			bool pt_fixed = false,
			GraphCurve* prev = nullptr,
			GraphCurve* next = nullptr,
			double cor = DEFAULT_COR,
			double period = DEFAULT_PERIOD
		);
		~BounceCurve() {}

		// カーブの値を生成
		double curve_function(double progress, double start, double end) const noexcept override;
		void clear() noexcept override;
		void reverse(bool fix_pt = false) noexcept override;

		void create_data(std::vector<byte>& data) const noexcept override;
		bool load_data(const byte* data, size_t size) noexcept override;

		// カーブから一意な整数値を生成
		int32_t encode() const noexcept override;
		// 整数値からカーブに変換
		bool decode(int32_t code) noexcept override;

		void draw_handle(
			mkaul::graphics::Graphics* p_graphics,
			const View& view,
			float thickness,
			float root_radius,
			float tip_radius,
			float tip_thickness,
			bool cutoff_line,
			const mkaul::ColorF& color = mkaul::ColorF{}
		) const noexcept override;

		bool is_handle_hovered(const mkaul::Point<double>& pt, const GraphView& view) const noexcept override;

		bool handle_check_hover(
			const mkaul::Point<double>& pt,
			const GraphView& view
		) noexcept override;

		bool handle_update(
			const mkaul::Point<double>& pt,
			const GraphView& view
		) noexcept override;
		void handle_end_control() noexcept override;

		ActivePoint pt_check_hover(const mkaul::Point<double>& pt, const GraphView& view) noexcept override;
		bool pt_begin_move(ActivePoint active_pt) noexcept override;
		ActivePoint pt_update(const mkaul::Point<double>& pt, const GraphView& view) noexcept override;
		bool pt_move(ActivePoint active_pt, const mkaul::Point<double>& pt) noexcept override;
	};
}