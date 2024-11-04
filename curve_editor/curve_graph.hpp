#pragma once

#include "curve_base.hpp"
#include "modifier.hpp"
#include <cereal/archives/binary.hpp>
#include <cereal/types/base_class.hpp>
#include <cereal/types/polymorphic.hpp>
#include <cereal/types/vector.hpp>



namespace cereal {
	template <class Archive>
	void save(Archive& archive, const mkaul::Point<double>& pt) {
		archive(
			pt.x,
			pt.y
		);
	}

	template <class Archive>
	void load(Archive& archive, mkaul::Point<double>& pt) {
		archive(
			pt.x,
			pt.y
		);
	}
}


namespace cved {
	// カーブ(グラフ)
	class GraphCurve : public Curve {
		GraphCurve* prev_;
		GraphCurve* next_;
		std::vector<std::unique_ptr<Modifier>> modifiers_;
		mkaul::Point<double> anchor_start_;
		mkaul::Point<double> anchor_end_;
		bool anchor_fixed_;

	public:
		enum class ActivePoint {
			Null,
			Start,
			End
		};

		// コンストラクタ
		GraphCurve(
			const mkaul::Point<double>& anchor_start = mkaul::Point{ 0., 0. },
			const mkaul::Point<double>& anchor_end = mkaul::Point{ 1., 1. },
			bool anchor_fixed = false,
			GraphCurve* prev = nullptr,
			GraphCurve* next = nullptr
		) noexcept :
			anchor_start_{ anchor_start },
			anchor_end_{ anchor_end },
			anchor_fixed_{ anchor_fixed },
			prev_{prev},
			next_{next}
		{}

		// コピーコンストラクタ
		GraphCurve(const GraphCurve& curve) noexcept :
			Curve{ curve },
			anchor_start_{ curve.anchor_start_ },
			anchor_end_{ curve.anchor_end_ },
			anchor_fixed_{ curve.anchor_fixed_ },
			prev_{ nullptr },
			next_{ nullptr },
			modifiers_{}
		{
			for (const auto& modifier : curve.modifiers_) {
				modifiers_.emplace_back(modifier->clone());
				modifiers_.back()->set_curve(this);
			}
		}

		[[nodiscard]] auto prev() const noexcept { return prev_; }
		[[nodiscard]] auto next() const noexcept { return next_; }
		virtual void set_prev(GraphCurve* p) noexcept { prev_ = p; }
		virtual void set_next(GraphCurve* p) noexcept { next_ = p; }

		[[nodiscard]] const auto& modifiers() const noexcept { return modifiers_; }
		[[nodiscard]] Modifier* get_modifier(size_t idx) const noexcept;
		void add_modifier(std::unique_ptr<Modifier>&& modifier) noexcept { modifiers_.emplace_back(std::move(modifier)); }
		void add_modifier(std::unique_ptr<Modifier>& modifier) noexcept { modifiers_.emplace_back(std::move(modifier)); }
		bool remove_modifier(size_t idx) noexcept;
		void clear_modifiers() noexcept { modifiers_.clear(); }
		void set_modifiers(std::vector<std::unique_ptr<Modifier>>& modifiers) noexcept { modifiers_ = std::move(modifiers); }
		void pop_modifier() noexcept { modifiers_.pop_back(); }

		[[nodiscard]] double get_value(double progress, double start, double end) const noexcept override;

		[[nodiscard]] const auto& anchor_start() const noexcept { return anchor_start_; }
		[[nodiscard]] const auto& anchor_end() const noexcept { return anchor_end_; }

		[[nodiscard]] double get_anchor_start_x() const noexcept { return anchor_start_.x; }
		[[nodiscard]] double get_anchor_start_y() const noexcept { return anchor_start_.y; }
		[[nodiscard]] double get_anchor_end_x() const noexcept { return anchor_end_.x; }
		[[nodiscard]] double get_anchor_end_y() const noexcept { return anchor_end_.y; }

		// アンカー参照・操作
		virtual void begin_move_anchor_start(bool bound = false) noexcept;
		virtual void begin_move_anchor_end(bool bound = false) noexcept;
		virtual void move_anchor_start(double x, double y, bool forced = false, bool bound = false) noexcept;
		void move_anchor_start(const mkaul::Point<double>& pt, bool forced = false, bool bound = false) noexcept { move_anchor_start(pt.x, pt.y, forced, bound); }
		virtual void move_anchor_end(double x, double y, bool forced = false, bool bound = false) noexcept;
		void move_anchor_end(const mkaul::Point<double>& pt, bool forced = false, bool bound = false) noexcept { move_anchor_end(pt.x, pt.y, forced, bound); }

		virtual void reverse(bool fix_pt = false) noexcept;

		[[nodiscard]] virtual nlohmann::json create_json() const noexcept override;
		virtual bool load_json(const nlohmann::json& data) noexcept override;

		template <class Archive>
		void save(Archive& archive, const std::uint32_t) const {
			archive(
				anchor_start_,
				anchor_end_,
				anchor_fixed_,
				modifiers_
			);
		}

		template <class Archive>
		void load(Archive& archive, const std::uint32_t) {
			archive(
				anchor_start_,
				anchor_end_,
				anchor_fixed_,
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