#include "handle_bezier.hpp"
#include "curve_bezier.hpp"
#include "config.hpp"
#include <cmath>



namespace cved {
	bool BezierHandle::key_state_snap() const noexcept {
		return ::GetAsyncKeyState(VK_SHIFT) & 0x8000 and not (::GetAsyncKeyState(VK_CONTROL) & 0x8000);
	}

	bool BezierHandle::key_state_lock_angle() const noexcept {
		return ::GetAsyncKeyState(VK_MENU) & 0x8000;
	}

	bool BezierHandle::key_state_lock_length() const noexcept {
		return ::GetAsyncKeyState(VK_CONTROL) & 0x8000 and not (::GetAsyncKeyState(VK_SHIFT) & 0x8000);
	}

	bool BezierHandle::check_flag_rise_snap() const noexcept {
		return !flag_prev_snap_ and key_state_snap();
	}

	bool BezierHandle::check_flag_rise_lock_angle() const noexcept {
		return !flag_prev_lock_angle_ and key_state_lock_angle();
	}

	bool BezierHandle::check_flag_rise_lock_length() const noexcept {
		return !flag_prev_lock_length_ and key_state_lock_length();
	}

	bool BezierHandle::check_flag_fall_snap() const noexcept {
		return flag_prev_snap_ and !key_state_snap();
	}

	bool BezierHandle::check_flag_fall_lock_angle() const noexcept {
		return flag_prev_lock_angle_ and !key_state_lock_angle();
	}

	bool BezierHandle::check_flag_fall_lock_length() const noexcept {
		return flag_prev_lock_length_ and !key_state_lock_length();
	}

	void BezierHandle::update_flags(const GraphView& view) noexcept {
		bool ks_snap = key_state_snap();
		bool ks_lock_angle = key_state_lock_angle();
		bool ks_lock_length = key_state_lock_length();

		if (check_flag_rise_snap()) {
			snap();
		}
		else if (check_flag_rise_lock_angle()) {
			lock_angle(view);
		}
		else if (check_flag_rise_lock_length()) {
			lock_length(view);
		}
		if (check_flag_fall_snap()) {
			unsnap();
		}
		if (check_flag_fall_lock_angle()) {
			unlock_angle();
		}
		if (check_flag_fall_lock_length()) {
			unlock_length();
		}

		flag_prev_snap_ = ks_snap;
		flag_prev_lock_angle_ = ks_lock_angle;
		flag_prev_lock_length_ = ks_lock_length;
	}

	void BezierHandle::snap() noexcept {
		double distance_start, distance_end;
		if (type_ == Type::Left) {
			distance_start = std::abs(pt_offset_.y());
			distance_end = std::abs(p_curve_->pt_end().y() - p_curve_->pt_start().y() - pt_offset_.y());
		}
		else {
			distance_start = std::abs(p_curve_->pt_end().y() - p_curve_->pt_start().y() + pt_offset_.y());
			distance_end = std::abs(pt_offset_.y());
		}
		snap_state_ = distance_end <= distance_start ? SnapState::SnapEnd : SnapState::SnapStart;
	}
	// スナップを解除
	void BezierHandle::unsnap() noexcept {
		snap_state_ = SnapState::Unsnapped;
	}

	// 角度を固定
	void BezierHandle::lock_angle(const GraphView& view) noexcept {
		buffer_angle_ = get_handle_angle(view);
		locked_angle_ = true;
	}

	// 角度の固定を解除
	void BezierHandle::unlock_angle() noexcept {
		buffer_angle_ = 0.;
		locked_angle_ = false;
	}

	// 長さを固定
	void BezierHandle::lock_length(const GraphView& view) noexcept {
		buffer_length_ = get_handle_length(view);
		locked_length_ = true;
	}

	// 長さの固定を解除
	void BezierHandle::unlock_length() noexcept {
		buffer_length_ = 0.;
		locked_length_ = false;
	}

	void BezierHandle::adjust_angle(const GraphView& view) noexcept {
		double length = get_handle_length(view);
		const GraphCurve* p_curve_neighbor = nullptr;
		mkaul::Point<double> pt_dest;

		if (type_ == Type::Left) {
			p_curve_neighbor = p_curve_->prev();
			if (!p_curve_neighbor) return;
			auto pt_origin = p_curve_->pt_start().pt();
			double slope = p_curve_neighbor->get_velocity(pt_origin.x, 0., 1.);
			double angle = std::atan(slope * view.scale_y() / view.scale_x());
			pt_dest = get_dest_pt(
				pt_origin + mkaul::Point<double>{
					length * std::cos(angle) / view.scale_x(),
					length * std::sin(angle) / view.scale_y()
				},
				view,
				true
			);
		}
		else {
			p_curve_neighbor = p_curve_->next();
			if (!p_curve_neighbor) return;
			auto pt_origin = p_curve_->pt_end().pt();
			// TODO: この0.000001は適当な値
			double slope = p_curve_neighbor->get_velocity(pt_origin.x + 0.000001, 0., 1.);
			double angle = std::atan(slope * view.scale_y() / view.scale_x());
			pt_dest = get_dest_pt(
				pt_origin - mkaul::Point<double>{
					length * std::cos(angle) / view.scale_x(),
					length * std::sin(angle) / view.scale_y()
				},
				view,
				true
			);
		}
		pt_offset_.move(pt_dest);
	}

	// 始点に移動
	void BezierHandle::move_to_root() noexcept {
		if (type_ == Type::Left) {
			pt_offset_.move(p_curve_->pt_start().pt());
		}
		else {
			pt_offset_.move(p_curve_->pt_end().pt());
		}
	}

	// ハンドルの角度を取得
	double BezierHandle::get_handle_angle(const GraphView& view) const noexcept {
		return std::atan2(pt_offset_.y() * view.scale_y(), pt_offset_.x() * view.scale_x());
	}

	// ハンドルの長さを取得
	double BezierHandle::get_handle_length(const GraphView& view) const noexcept {
		double x = pt_offset_.x();
		double y = pt_offset_.y();

		return std::sqrt(x * x * view.scale_x() * view.scale_x() + y * y * view.scale_y() * view.scale_y());
	}

	// 指定したポイントの基準点からの角度を取得
	double BezierHandle::get_cursor_angle(
		const mkaul::Point<double>& pt,
		const GraphView& view
	) const noexcept {
		const mkaul::Point pt_origin = type_ == Type::Left ? p_curve_->pt_start().pt() : p_curve_->pt_end().pt();
		return std::atan2(
			(pt.y - pt_origin.y) * view.scale_y(),
			(pt.x - pt_origin.x) * view.scale_x()
		);
	}

	// 指定したポイントの基準点からの長さを取得
	double BezierHandle::get_cursor_length(
		const mkaul::Point<double>& pt,
		const GraphView& view
	) const noexcept {
		const mkaul::Point pt_origin = type_ == Type::Left ? p_curve_->pt_start().pt() : p_curve_->pt_end().pt();
		return std::sqrt(
			(pt.x - pt_origin.x) * (pt.x - pt_origin.x) * view.scale_x() * view.scale_x() +
			(pt.y - pt_origin.y) * (pt.y - pt_origin.y) * view.scale_y() * view.scale_y()
		);
	}

	// 移動先のハンドルの座標を取得
	mkaul::Point<double> BezierHandle::get_dest_pt(
		const mkaul::Point<double>& pt,
		const GraphView& view,
		bool keep_angle
	) const noexcept {
		mkaul::Point<double> pt_origin, pt_opposite, ret;

		if (type_ == Type::Left) {
			pt_origin = p_curve_->pt_start().pt();
			pt_opposite = p_curve_->pt_end().pt();
		}
		else {
			pt_origin = p_curve_->pt_end().pt();
			pt_opposite = p_curve_->pt_start().pt();
		}

		if (snap_state_ != SnapState::Unsnapped) {
			ret.x = pt.x - pt_origin.x;
			if (snap_state_ == SnapState::SnapStart) {
				ret.y = p_curve_->pt_start().y() - pt_origin.y;
			}
			else {
				ret.y = p_curve_->pt_end().y() - pt_origin.y;
			}
		}
		else if (locked_angle_) {
			double length = get_cursor_length(pt, view);
			ret = mkaul::Point{
				length * std::cos(buffer_angle_) / view.scale_x(),
				length * std::sin(buffer_angle_) / view.scale_y()
			};
		}
		else if (locked_length_) {
			double angle = get_cursor_angle(pt, view);
			ret = mkaul::Point{
				buffer_length_ * std::cos(angle) / view.scale_x(),
				buffer_length_ * std::sin(angle) / view.scale_y()
			};
		}
		else {
			ret = pt - pt_origin;
		}
		limit_range(ret, locked_angle_ or locked_length_ or keep_angle);
		return ret;
	}

	void BezierHandle::limit_range(
		mkaul::Point<double>& pt_offset,
		bool keep_angle
	) const noexcept {
		mkaul::Point<double> pt_origin, pt_opposite;

		if (type_ == Type::Left) {
			pt_origin = p_curve_->pt_start().pt();
			pt_opposite = p_curve_->pt_end().pt();
		}
		else {
			pt_origin = p_curve_->pt_end().pt();
			pt_opposite = p_curve_->pt_start().pt();
		}
		double width = pt_opposite.x - pt_origin.x;
		// ハンドルが範囲外にある場合
		if (pt_offset.x * mkaul::sign(width) < 0.) {
			pt_offset.x = 0.;
		}
		else if (std::abs(width) < std::abs(pt_offset.x)) {
			double tmp = pt_offset.x;
			pt_offset.x = width;
			if (keep_angle) {
				pt_offset.y *= width / tmp;
			}
		}
	}

	// カーソルがハンドルにホバーしているか
	bool BezierHandle::is_hovered(const mkaul::Point<double>& pt, const GraphView& view) const noexcept {
		const mkaul::Point pt_origin = type_ == Type::Left ? p_curve_->pt_start().pt() : p_curve_->pt_end().pt();
		return pt_offset_.is_hovered(pt - pt_origin, view);
	}

	// ハンドルの移動を開始する
	bool BezierHandle::check_hover(
		const mkaul::Point<double>& pt,
		const GraphView& view
	) noexcept {
		const mkaul::Point pt_origin = type_ == Type::Left ? p_curve_->pt_start().pt() : p_curve_->pt_end().pt();
		bool ks_snap = key_state_snap();
		bool ks_lock_angle = key_state_lock_angle();
		bool ks_lock_length = key_state_lock_length();

		flag_prev_snap_ = ks_snap;
		flag_prev_lock_angle_ = ks_lock_angle;
		flag_prev_lock_length_ = ks_lock_length;

		if (pt_offset_.check_hover(pt - pt_origin, view)) {
			if (ks_snap) {
				snap();
			}
			else if (ks_lock_angle) {
				lock_angle(view);
			}
			else if (ks_lock_length) {
				lock_length(view);
			}
			if (handle_opposite_ and global::config.get_align_handle()) {
				handle_opposite_->lock_length(view);
			}
			return true;
		}
		else return false;
	}

	void BezierHandle::begin_move(
		const GraphView& view
	) noexcept {
		bool ks_snap = key_state_snap();
		bool ks_lock_angle = key_state_lock_angle();
		bool ks_lock_length = key_state_lock_length();

		flag_prev_snap_ = ks_snap;
		flag_prev_lock_angle_ = ks_lock_angle;
		flag_prev_lock_length_ = ks_lock_length;

		if (ks_lock_length) {
			lock_length(view);
		}
		else if (ks_snap) {
			snap();
		}
		else if (ks_lock_angle) {
			lock_angle(view);
		}
		if (handle_opposite_ and global::config.get_align_handle()) {
			handle_opposite_->lock_length(view);
		}
	}

	bool BezierHandle::update(
		const mkaul::Point<double>& pt,
		const GraphView& view
	) noexcept {

		if (pt_offset_.is_controlled()) {
			update_flags(view);
			pt_offset_.move(get_dest_pt(pt, view));
			if (handle_opposite_ and global::config.get_align_handle()) {
				const mkaul::Point pt_origin = type_ == Type::Left ? p_curve_->pt_start().pt() : p_curve_->pt_end().pt();
				handle_opposite_->move(pt_origin - pt_offset_.pt(), view, true);
			}
			return true;
		}
		else return false;
	}

	void BezierHandle::move(
		const mkaul::Point<double>& pt,
		const GraphView& view,
		bool aligned,
		bool moved_symmetrically
	) noexcept {
		if (!aligned and !moved_symmetrically) {
			update_flags(view);
		}
		pt_offset_.move(get_dest_pt(pt, view));
		// ポイントの向かいのハンドルを連動させる
		if (!aligned and handle_opposite_ and global::config.get_align_handle()) {
			const mkaul::Point pt_origin = type_ == Type::Left ? p_curve_->pt_start().pt() : p_curve_->pt_end().pt();
			handle_opposite_->move(pt_origin - pt_offset_.pt(), view, true);
		}
	}

	void BezierHandle::set_position(
		const mkaul::Point<double>& pt
	) noexcept {
		mkaul::Point<double> ret;
		const mkaul::Point pt_origin = type_ == Type::Left ? p_curve_->pt_start().pt() : p_curve_->pt_end().pt();

		ret = pt - pt_origin;
		limit_range(ret, true);
		pt_offset_.move(ret);
	}

	// ハンドルの移動を終了する
	void BezierHandle::end_move() noexcept {
		unsnap();
		unlock_angle();
		unlock_length();
		if (handle_opposite_) {
			handle_opposite_->unlock_length();
		}
	}

	void BezierHandle::end_control() noexcept {
		pt_offset_.end_control();
		end_move();
	}
}