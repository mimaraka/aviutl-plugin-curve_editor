#pragma once

#include <mkaul/point.hpp>


namespace curve_editor {
	class PositionConverter {
	public:
		virtual void apply(mkaul::Point<double>& pos_rel) const noexcept = 0;
	};

	class XRangeLimitter : public PositionConverter {
		const mkaul::Point<double>& anchor_origin_;
		const mkaul::Point<double>& anchor_opposite_;
		bool keep_angle_ = false;

	public:
		XRangeLimitter(const mkaul::Point<double>& anchor_origin, const mkaul::Point<double>& anchor_opposite) noexcept :
			anchor_origin_{ anchor_origin }, anchor_opposite_{ anchor_opposite }
		{}

		void set_keep_angle(bool keep_angle) noexcept { keep_angle_ = keep_angle; }

		void apply(mkaul::Point<double>& pos_rel) const noexcept override {
			const auto ret_x = mkaul::clamp(pos_rel.x, 0., anchor_opposite_.x - anchor_origin_.x);
			auto ret_y = pos_rel.y;
			if (keep_angle_) {
				const auto angle = std::atan2(pos_rel.y, pos_rel.x);
				if (!mkaul::real_equal(angle, std::numbers::pi * 0.5) and !mkaul::real_equal(angle, -std::numbers::pi * 0.5)) {
					ret_y = ret_x * std::tan(angle);
				}
			}
			pos_rel.x = ret_x;
			pos_rel.y = ret_y;
		}
	};

	class Locker : public PositionConverter {
	protected:
		bool locked_ = false;

	public:
		virtual void lock() noexcept { locked_ = true; }
		void unlock() noexcept { locked_ = false; }
		[[nodiscard]] bool is_locked() const noexcept { return locked_; }
	};


	class TangentLocker : public Locker {
		double tangent_ = 0.;

	public:
		void lock(const mkaul::Point<double>& pos_rel) noexcept {
			if (pos_rel.x != 0.) {
				__super::lock();
				tangent_ = pos_rel.y / pos_rel.x;
			}
		}

		void apply(mkaul::Point<double>& pos_rel) const noexcept override {
			if (locked_) {
				pos_rel.y = pos_rel.x * tangent_;
			}
		}
	};

	class LengthLocker : public Locker {
		const mkaul::Point<double>& anchor_origin_;
		const mkaul::Point<double>& anchor_opposite_;
		double length_ = 0.;
		double scale_x_ = 1.;
		double scale_y_ = 1.;

	public:
		LengthLocker(const mkaul::Point<double>& anchor_origin, const mkaul::Point<double>& anchor_opposite) noexcept :
			anchor_origin_{ anchor_origin }, anchor_opposite_{ anchor_opposite }
		{}

		void lock(const mkaul::Point<double>& pos_rel) noexcept {
			__super::lock();
			length_ = std::hypot(pos_rel.x * scale_x_, pos_rel.y * scale_y_);
		}

		void set_scale(double scale_x, double scale_y) noexcept {
			scale_x_ = scale_x;
			scale_y_ = scale_y;
		}

		void apply(mkaul::Point<double>& pos_rel) const noexcept override {
			if (locked_) {
				const auto current_length = std::hypot(pos_rel.x * scale_x_, pos_rel.y * scale_y_);
				if (current_length == 0.) {
					auto tmp = anchor_opposite_.x - anchor_origin_.x;
					pos_rel.x = length_ / scale_x_ * tmp / std::abs(tmp);
				}
				else {
					const auto scale = length_ / current_length;
					pos_rel.x *= scale;
					pos_rel.y *= scale;
				}
			}
		}
	};

	class Snapper : public PositionConverter {
		const mkaul::Point<double>& anchor_origin_;
		const mkaul::Point<double>& anchor_opposite_;
		enum class SnapState {
			UnSnapped,
			SnapOrigin,
			SnapOpposite
		} stap_state_ = SnapState::UnSnapped;

	public:
		Snapper(const mkaul::Point<double>& anchor_origin, const mkaul::Point<double>& anchor_opposite) noexcept :
			anchor_origin_{ anchor_origin }, anchor_opposite_{ anchor_opposite }
		{}

		void apply(mkaul::Point<double>& pos_rel) const noexcept override {
			if (stap_state_ == SnapState::SnapOrigin) {
				pos_rel.y = 0.;
			}
			else if (stap_state_ == SnapState::SnapOpposite) {
				pos_rel.y = anchor_opposite_.y - anchor_origin_.y;
			}
		}

		void snap(const mkaul::Point<double>& pos_rel) noexcept {
			auto distance_origin_y = std::abs(pos_rel.y);
			auto distance_opposite_y = std::abs((anchor_opposite_.y - anchor_origin_.y) - pos_rel.y);
			if (distance_origin_y < distance_opposite_y) {
				stap_state_ = SnapState::SnapOrigin;
			}
			else {
				stap_state_ = SnapState::SnapOpposite;
			}
		}
		void unsnap() noexcept { stap_state_ = SnapState::UnSnapped; }
	};
} // namespace curve_editor