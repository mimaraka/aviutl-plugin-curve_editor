#pragma once

#include "curve.hpp"
#include <cereal/types/base_class.hpp>
#include <cereal/types/vector.hpp>
#include <cereal/archives/binary.hpp>
#include <cereal/types/polymorphic.hpp>
#include "control_point.hpp"
#include "modifier.hpp"
#include "view_graph.hpp"



namespace cved {
	// カーブ(グラフ)
	class GraphCurve : public Curve {
		GraphCurve* prev_;
		GraphCurve* next_;
		std::vector<std::unique_ptr<Modifier>> modifiers_;

	protected:
		ControlPoint pt_start_;
		ControlPoint pt_end_;

	public:
		enum class ActivePoint {
			Null,
			Start,
			End
		};

		// コンストラクタ
		GraphCurve(
			const mkaul::Point<double>& pt_start = mkaul::Point{ 0., 0. },
			const mkaul::Point<double>& pt_end = mkaul::Point{ 1., 1. },
			bool pt_fixed = false,
			GraphCurve* prev = nullptr,
			GraphCurve* next = nullptr
		) noexcept :
			pt_start_{ pt_start, pt_fixed },
			pt_end_{ pt_end, pt_fixed },
			prev_{prev},
			next_{next}
		{}

		// コピーコンストラクタ
		GraphCurve(const GraphCurve& curve) noexcept :
			pt_start_{ curve.pt_start_ },
			pt_end_{ curve.pt_end_ },
			prev_{ nullptr },
			next_{ nullptr }
		{}

		auto prev() const noexcept { return prev_; }
		auto next() const noexcept { return next_; }
		virtual void set_prev(GraphCurve* p) noexcept { prev_ = p; }
		virtual void set_next(GraphCurve* p) noexcept { next_ = p; }

		const auto& modifiers() const noexcept { return modifiers_; }
		Modifier* get_modifier(size_t idx) const noexcept;
		void add_modifier(std::unique_ptr<Modifier>&& modifier) noexcept { modifiers_.emplace_back(std::move(modifier)); }
		void add_modifier(std::unique_ptr<Modifier>& modifier) noexcept { modifiers_.emplace_back(std::move(modifier)); }
		bool remove_modifier(size_t idx) noexcept;
		void clear_modifiers() noexcept { modifiers_.clear(); }
		void set_modifiers(std::vector<std::unique_ptr<Modifier>>& modifiers) noexcept { modifiers_ = std::move(modifiers); }
		void pop_modifier() noexcept { modifiers_.pop_back(); }

		double get_value(double progress, double start, double end) const noexcept override;

		void draw_curve(
			mkaul::graphics::Graphics* p_graphics,
			const View& view,
			float thickness = 1.f,
			float drawing_interval = 1.f,
			const mkaul::ColorF& color = mkaul::ColorF{},
			bool velocity = false
		) const noexcept;

		virtual void draw_handle(
			mkaul::graphics::Graphics* p_graphics,
			const View& view,
			float thickness,
			float root_radius,
			float tip_radius,
			float tip_thickness,
			bool cutoff_line,
			const mkaul::ColorF& color = mkaul::ColorF{}
		) const noexcept = 0;

		void draw_pt(
			mkaul::graphics::Graphics* p_graphics,
			const View& view,
			float radius,
			const mkaul::ColorF& color = mkaul::ColorF{}
		) const noexcept;

		const auto& pt_start() const noexcept { return pt_start_; }
		const auto& pt_end() const noexcept { return pt_end_; }

		virtual void reverse(bool fix_pt = false) noexcept;

		bool is_hovered(const mkaul::Point<double>& pt, const GraphView& view) const noexcept;
		virtual bool is_pt_hovered(const mkaul::Point<double>& pt, const GraphView& view) const noexcept;
		virtual bool is_handle_hovered(const mkaul::Point<double>& pt, const GraphView& view) const noexcept = 0;

		virtual bool handle_check_hover(
			const mkaul::Point<double>& pt,
			const GraphView& view
		) noexcept = 0;

		virtual bool handle_update(
			const mkaul::Point<double>& pt,
			const GraphView& view
		) noexcept = 0;

		virtual void handle_end_control() noexcept = 0;

		virtual ActivePoint pt_check_hover(const mkaul::Point<double>& pt, const GraphView& view) noexcept = 0;
		virtual bool pt_begin_move(ActivePoint active_pt) noexcept = 0;
		virtual ActivePoint pt_update(const mkaul::Point<double>& pt, const GraphView& view) noexcept = 0;
		virtual bool pt_move(ActivePoint active_pt, const mkaul::Point<double>& pt) noexcept;
		virtual void pt_end_move() noexcept {};
		virtual void pt_end_control() noexcept;

		template <class Archive>
		void save(Archive& archive, const std::uint32_t) const {
			archive(
				pt_start_,
				pt_end_,
				modifiers_
			);
		}

		template <class Archive>
		void load(Archive& archive, const std::uint32_t) {
			archive(
				pt_start_,
				pt_end_,
				modifiers_
			);

			for (auto& modifier : modifiers_) {
				modifier->set_curve(this);
			}
		}
	};
}

CEREAL_CLASS_VERSION(cved::GraphCurve, 0)
CEREAL_REGISTER_TYPE(cved::GraphCurve)
CEREAL_REGISTER_POLYMORPHIC_RELATION(cved::Curve, cved::GraphCurve)