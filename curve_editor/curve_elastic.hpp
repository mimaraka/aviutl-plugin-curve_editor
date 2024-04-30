#pragma once

#include "curve_graph_numeric.hpp"
#include "handle_elastic_amp.hpp"
#include "handle_elastic_freq_decay.hpp"



namespace cved {
	// カーブ(振動)
	class ElasticCurve : public NumericGraphCurve {
		static constexpr double DEFAULT_AMP = 1.;
		static constexpr double DEFAULT_FREQ = 8.;
		static constexpr double DEFAULT_DECAY = 6.;
		ElasticAmpHandle handle_amp_;
		ElasticFreqDecayHandle handle_freq_decay_;
		double amp_;
		double freq_;
		double decay_;

	public:
		// コンストラクタ
		ElasticCurve(
			const mkaul::Point<double>& pt_start = mkaul::Point{ 0., 0. },
			const mkaul::Point<double>& pt_end = mkaul::Point{ 1., 1. },
			uint32_t sampling_resolution = 0u,
			uint32_t quantization_resolution = 0u,
			bool pt_fixed = false,
			GraphCurve* prev = nullptr,
			GraphCurve* next = nullptr,
			double amp = DEFAULT_AMP,
			double freq = DEFAULT_FREQ,
			double decay = DEFAULT_DECAY
		) noexcept;

		// カーブの値を取得
		double curve_function(double progress, double start, double end) const noexcept override;
		void clear() noexcept override;
		void reverse() noexcept override;

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