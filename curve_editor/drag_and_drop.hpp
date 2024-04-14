#pragma once

#include <array>
#include "trackbar_button.hpp"



namespace cved {
	class DragAndDrop {
		bool dragging_ = false;
		std::array<TrackbarButton, ExEdit::Object::MAX_TRACK> buttons_;
		std::unique_ptr<mkaul::graphics::Graphics> p_graphics_;

		static void get_applied_track_idcs(int32_t obj_idx, int32_t track_idx, std::vector<int32_t>& ret) noexcept;
		static void apply_easing_to_track(int32_t obj_idx, int32_t track_idx) noexcept;
		// X, Y, Z等のトラックバーではすべて同時にイージングが設定される
		static void apply_easing_to_tracks(int32_t obj_idx, int32_t track_idx) noexcept;

	public:
		bool init() noexcept;
		void exit() noexcept;

		bool drag() noexcept;
		bool is_dragging() const noexcept { return dragging_; }
		bool drop() noexcept;
		void highlight() const noexcept;
	};
}