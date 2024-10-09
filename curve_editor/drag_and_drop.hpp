#pragma once

#include "trackbar_button.hpp"
#include <array>



namespace cved {
	class DragAndDrop {
		ID2D1Factory* p_factory_ = nullptr;
		ID2D1DCRenderTarget* p_render_target_ = nullptr;
		bool dragging_ = false;
		bool d2d_init_failed_ = false;
		std::array<TrackbarButton, ExEdit::Object::MAX_TRACK> buttons_;
		std::vector<int32_t> track_idcs_buffer_;

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
		void update() noexcept;
	};
}