#pragma once

#include "trackbar_button.hpp"



namespace cved {
	class DragAndDrop {
		bool dragging_ = false;
		TrackbarButton buttons_[ExEdit::Object::MAX_TRACK];
		static void apply_track_mode(ExEdit::Object* p_object, size_t track_idx) noexcept;

	public:
		bool init() noexcept;

		bool drag() noexcept;
		bool is_dragging() const noexcept { return dragging_; }
		bool drop() noexcept;
	};
}