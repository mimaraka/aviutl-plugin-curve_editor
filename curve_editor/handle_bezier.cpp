#include "curve_id_manager.hpp"
#include "handle_bezier.hpp"



namespace cved {
	bool BezierHandle::is_default(double curve_width, double curve_height) const noexcept {
		bool is_handle_left_x_default = mkaul::real_equal(
			handle_left_.x, DEFAULT_HANDLE_XY * curve_width
		);
		bool is_handle_left_y_default = mkaul::real_equal(
			handle_left_.y, DEFAULT_HANDLE_XY * curve_height
		);
		bool is_handle_right_x_default = mkaul::real_equal(
			handle_right_.x, -DEFAULT_HANDLE_XY * curve_width
		);
		bool is_handle_right_y_default = mkaul::real_equal(
			handle_right_.y, -DEFAULT_HANDLE_XY * curve_height
		);

		return is_handle_left_x_default and is_handle_left_y_default and
			is_handle_right_x_default and is_handle_right_y_default;
	}

	void BezierHandle::on_anchor_begin_move() noexcept {
		handle_left_buffer_ = handle_left_;
		handle_right_buffer_ = handle_right_;
	}

	void BezierHandle::on_anchor_move(const mkaul::Point<double>& anchor_start, const mkaul::Point<double>& anchor_end) noexcept {
		move_handle_left(anchor_start + handle_left_buffer_, anchor_start, anchor_end, true);
		move_handle_right(anchor_end + handle_right_buffer_, anchor_start, anchor_end, true);
	}

	void BezierHandle::begin_move_handle_left(double scale_x, double scale_y, const mkaul::Point<double>* p_prev_handle_right) noexcept {
		if (p_prev_handle_right) {
			scale_x_buffer_ = scale_x;
			scale_y_buffer_ = scale_y;
			auto scaled_prev_offset_x = p_prev_handle_right->x * scale_x;
			auto scaled_prev_offset_y = p_prev_handle_right->y * scale_y;
			length_buffer_ = std::sqrt(scaled_prev_offset_x * scaled_prev_offset_x + scaled_prev_offset_y * scaled_prev_offset_y);
		}
	}

	void BezierHandle::begin_move_handle_right(double scale_x, double scale_y, const mkaul::Point<double>* p_next_handle_left) noexcept {
		if (p_next_handle_left) {
			scale_x_buffer_ = scale_x;
			scale_y_buffer_ = scale_y;
			auto scaled_next_offset_x = p_next_handle_left->x * scale_x;
			auto scaled_next_offset_y = p_next_handle_left->y * scale_y;
			length_buffer_ = std::sqrt(scaled_next_offset_x * scaled_next_offset_x + scaled_next_offset_y * scaled_next_offset_y);
		}
	}

	void BezierHandle::move_handle_left(
		const mkaul::Point<double>& pt,
		const mkaul::Point<double>& anchor_start,
		const mkaul::Point<double>& anchor_end,
		bool keep_angle,
		mkaul::Point<double>* p_prev_handle_right
	) noexcept {
		auto offset_x = pt.x - anchor_start.x;
		auto offset_y = pt.y - anchor_start.y;
		const auto ret_x = mkaul::clamp(offset_x, 0., anchor_end.x - anchor_start.x);
		double ret_y;
		if (keep_angle) {
			auto angle = std::atan2(offset_y, offset_x);
			ret_y = ret_x * std::tan(angle);
		}
		else {
			ret_y = offset_y;
		}
		handle_left_ = mkaul::Point{ ret_x, ret_y };

		if (p_prev_handle_right) {
			const auto scaled_angle = std::atan2(-scale_y_buffer_ * ret_y, scale_x_buffer_ * ret_x);
			const auto scaled_offset_x = length_buffer_ * std::cos(scaled_angle + std::numbers::pi);
			const auto scaled_offset_y = -length_buffer_ * std::sin(scaled_angle + std::numbers::pi);
			p_prev_handle_right->x = scaled_offset_x / scale_x_buffer_;
			p_prev_handle_right->y = scaled_offset_y / scale_y_buffer_;
		}
	}

	void BezierHandle::move_handle_right(
		const mkaul::Point<double>& pt,
		const mkaul::Point<double>& anchor_start,
		const mkaul::Point<double>& anchor_end,
		bool keep_angle,
		mkaul::Point<double>* p_next_handle_left
	) noexcept {
		auto offset_x = pt.x - anchor_end.x;
		auto offset_y = pt.y - anchor_end.y;
		auto ret_x = mkaul::clamp(offset_x, anchor_start.x - anchor_end.x, 0.);
		double ret_y;
		if (keep_angle) {
			auto angle = std::atan2(offset_y, offset_x);
			ret_y = ret_x * std::tan(angle);
		}
		else {
			ret_y = offset_y;
		}
		handle_right_ = mkaul::Point{ ret_x, ret_y };

		if (p_next_handle_left) {
			const auto scaled_angle = std::atan2(-scale_y_buffer_ * ret_y, scale_x_buffer_ * ret_x);
			const auto scaled_offset_x = length_buffer_ * std::cos(scaled_angle + std::numbers::pi);
			const auto scaled_offset_y = -length_buffer_ * std::sin(scaled_angle + std::numbers::pi);
			p_next_handle_left->x = scaled_offset_x / scale_x_buffer_;
			p_next_handle_left->y = scaled_offset_y / scale_y_buffer_;
		}
	}
}
} // namespace cved