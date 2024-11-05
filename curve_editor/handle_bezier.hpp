#pragma once

#include "cereal_mkaul_point.hpp"
#include "key_state_observer.hpp"
#include <cereal/cereal.hpp>
#include <mkaul/point.hpp>



namespace cved {
	class BezierHandle {
		mkaul::Point<double> handle_left_;
		mkaul::Point<double> handle_right_;
		mkaul::Point<double> handle_left_buffer_;
		mkaul::Point<double> handle_right_buffer_;
		double length_buffer_ = 0.;
		double scale_x_buffer_ = 0.;
		double scale_y_buffer_ = 0.;
		KeyStateObserver key_state_ctrl_{ VK_CONTROL };
		KeyStateObserver key_state_shift_{ VK_SHIFT };
		KeyStateObserver key_state_alt_{ VK_MENU };

	public:
		static constexpr double DEFAULT_HANDLE_XY = 0.3;

		BezierHandle(
			const mkaul::Point<double>& handle_left = mkaul::Point{ DEFAULT_HANDLE_XY, DEFAULT_HANDLE_XY },
			const mkaul::Point<double>& handle_right = mkaul::Point{ -DEFAULT_HANDLE_XY, -DEFAULT_HANDLE_XY }
		) noexcept : 
			handle_left_{ handle_left }, handle_right_{ handle_right }
		{}

		void clear(double curve_width, double curve_height) noexcept {
			handle_left_ = mkaul::Point{ DEFAULT_HANDLE_XY * curve_width, DEFAULT_HANDLE_XY * curve_height };
			handle_right_ = mkaul::Point{ -DEFAULT_HANDLE_XY * curve_width, -DEFAULT_HANDLE_XY * curve_height };
		}

		[[nodiscard]] bool is_default(double curve_width, double curve_height) const noexcept;

		void reverse() noexcept {
			std::swap(handle_left_, handle_right_);
			handle_left_ = -handle_left_;
			handle_right_ = -handle_right_;
		}

		[[nodiscard]] const auto& left_offset() const noexcept { return handle_left_; }
		[[nodiscard]] const auto& right_offset() const noexcept { return handle_right_; }

		void on_anchor_begin_move() noexcept;
		void on_anchor_move(const mkaul::Point<double>& anchor_start, const mkaul::Point<double>& anchor_end) noexcept;

		// ハンドルを移動する
		void begin_move_handle_left(double scale_x, double scale_y, const mkaul::Point<double>* p_prev_handle_right = nullptr) noexcept;
		void begin_move_handle_right(double scale_x, double scale_y, const mkaul::Point<double>* p_next_handle_left = nullptr) noexcept;
		void move_handle_left(
			const mkaul::Point<double>& pt,
			const mkaul::Point<double>& anchor_start,
			const mkaul::Point<double>& anchor_end,
			bool keep_angle = false,
			mkaul::Point<double>* p_prev_handle_right = nullptr
		) noexcept;
		void move_handle_right(
			const mkaul::Point<double>& pt,
			const mkaul::Point<double>& anchor_start,
			const mkaul::Point<double>& anchor_end,
			bool keep_angle = false,
			mkaul::Point<double>* p_next_handle_left = nullptr
		) noexcept;

		template<class Archive>
		void save(Archive& archive, const std::uint32_t) const {
			archive(
				handle_left_,
				handle_right_
			);
		}

		template<class Archive>
		void load(Archive& archive, const std::uint32_t) {
			archive(
				handle_left_,
				handle_right_
			);
		}
	};
}