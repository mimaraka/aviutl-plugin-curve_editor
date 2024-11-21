#include "handle_bezier.hpp"



namespace curve_editor {
	void BezierHandle::update_key_state() noexcept {
		ks_snap_.update();
		ks_lock_tangent_.update();
		ks_lock_length_.update();
		ks_move_symmetrically_.update();

		if (ks_lock_tangent_.is_key_down()) {
			tangent_locker_.lock(handle_);
		}
		else if (ks_lock_tangent_.is_key_up()) {
			tangent_locker_.unlock();
		}

		if (ks_lock_length_.is_key_down()) {
			length_locker_.lock(handle_);
		}
		else if (ks_lock_length_.is_key_up()) {
			length_locker_.unlock();
		}

		if (ks_snap_.is_key_down()) {
			snapper_.snap(handle_);
		}
		else if (ks_snap_.is_key_up()) {
			snapper_.unsnap();
		}

		if (ks_move_symmetrically_.is_key_down()) {
			length_locker_.unlock();
			snapper_.unsnap();
		}
		else if (ks_move_symmetrically_.is_key_up()) {
			if (ks_lock_length_.is_key_pressed()) {
				length_locker_.lock(handle_);
			}
			if (ks_snap_.is_key_pressed()) {
				snapper_.snap(handle_);
			}
		}
	}

	void BezierHandle::move_rel(const mkaul::Point<double>& pos_rel_, bool keep_angle, bool enable_key) noexcept {
		if (enable_key) {
			update_key_state();
		}
		x_range_limitter_.set_keep_angle(keep_angle);
		handle_ = pos_rel_;
		x_range_limitter_.apply(handle_);
		tangent_locker_.apply(handle_);
		length_locker_.apply(handle_);
		snapper_.apply(handle_);
		if (length_locker_.is_locked()) {
			x_range_limitter_.set_keep_angle(true);
			x_range_limitter_.apply(handle_);
		}
	}
} // namespace curve_editor