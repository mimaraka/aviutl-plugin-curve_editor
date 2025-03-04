#pragma once

#include "cereal_mkaul_point.hpp"
#include "key_state_observer.hpp"
#include "position_converter.hpp"
#include <mkaul/point.hpp>



namespace curve_editor {
	class BezierHandle {
		const mkaul::Point<double>& anchor_origin_;
		const mkaul::Point<double>& anchor_opposite_;
		mkaul::Point<double> handle_;
		double scale_x_ = 1.;
		double scale_y_ = 1.;
		XRangeLimitter x_range_limitter_;
		TangentLocker tangent_locker_;
		LengthLocker length_locker_;
		Snapper snapper_;
		KeyStateObserver ks_snap_{ VK_SHIFT };
		KeyStateObserver ks_lock_tangent_{ VK_MENU };
		KeyStateObserver ks_lock_length_{ VK_CONTROL };
		KeyStateObserver ks_move_symmetrically_{ VK_CONTROL, VK_SHIFT };

	public:
		static constexpr double DEFAULT_HANDLE_RATIO = 0.3;

		BezierHandle(
			const mkaul::Point<double>& anchor_origin,
			const mkaul::Point<double>& anchor_opposite,
			const mkaul::Point<double>& handle = { 0., 0. }
		) :
			anchor_origin_{ anchor_origin },
			anchor_opposite_{ anchor_opposite },
			handle_{ handle },
			x_range_limitter_{ anchor_origin, anchor_opposite },
			length_locker_{ anchor_origin, anchor_opposite },
			snapper_{ anchor_origin, anchor_opposite }
		{}

		void clear() noexcept {
			handle_ = {
				(anchor_opposite_.x - anchor_origin_.x) * DEFAULT_HANDLE_RATIO,
				(anchor_opposite_.y - anchor_origin_.y) * DEFAULT_HANDLE_RATIO
			};
		}

		[[nodiscard]] bool is_default() const noexcept {
			return mkaul::real_equal(handle_.x, (anchor_opposite_.x - anchor_origin_.x) * DEFAULT_HANDLE_RATIO) and
				mkaul::real_equal(handle_.y, (anchor_opposite_.y - anchor_origin_.y) * DEFAULT_HANDLE_RATIO);
		}

		auto pos_abs() const noexcept { return anchor_origin_ + handle_; }
		const auto& pos_rel() const noexcept { return handle_; }

		// TODO: 良くない
		auto scale_x() const noexcept { return scale_x_; }
		auto scale_y() const noexcept { return scale_y_; }

		void update_key_state() noexcept;

		void lock_length() noexcept { length_locker_.lock(handle_); }
		void unlock_length() noexcept { length_locker_.unlock(); }

		void lock_tangent() noexcept { tangent_locker_.lock(handle_); }
		void unlock_tangent() noexcept { tangent_locker_.unlock(); }

		auto is_key_down_move_symmetrically() const noexcept { return ks_move_symmetrically_.is_key_down(); }
		auto is_key_pressed_move_symmetrically() const noexcept { return ks_move_symmetrically_.is_key_pressed(); }
		auto is_key_up_move_symmetrically() const noexcept { return ks_move_symmetrically_.is_key_up(); }

		void begin_move(double scale_x, double scale_y, bool enable_key = false) noexcept {
			scale_x_ = scale_x;
			scale_y_ = scale_y;
			length_locker_.set_scale(scale_x, scale_y);
			length_locker_.unlock();
			tangent_locker_.unlock();
			snapper_.unsnap();
			if (enable_key) {
				update_key_state();
			}
		}

		void move(const mkaul::Point<double>& pos_abs_, bool keep_angle = false, bool enable_key = false) noexcept { move_rel(pos_abs_ - anchor_origin_, keep_angle, enable_key); }
		void move_rel(const mkaul::Point<double>& pos_rel_, bool keep_angle = false, bool enable_key = false) noexcept;

		void end_move() noexcept {
			tangent_locker_.unlock();
			length_locker_.unlock();
			snapper_.unsnap();
		}

		template<class Archive>
		void save(Archive& archive, const std::uint32_t) const {
			archive(handle_);
		}

		template<class Archive>
		void load(Archive& archive, const std::uint32_t) {
			archive(handle_);
		}
	};
} // namespace curve_editor