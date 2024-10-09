#pragma once

#include "curve_graph.hpp"
#include "menu.hpp"
#include <optional>



namespace cved {
	class CurveSegmentMenu : public Menu {
		HMENU submenu_segment_type_;
		void update_state(uint32_t idx) noexcept;
		bool callback(uint32_t idx, uint16_t id) noexcept;
		std::optional<uint32_t> get_idx(GraphCurve* p_segment) noexcept;

	public:
		CurveSegmentMenu(HINSTANCE hinst);

		HMENU get_handle(uint32_t idx) noexcept;
		int show(
			GraphCurve* p_segment,
			HWND hwnd,
			UINT flags = NULL,
			const mkaul::Point<LONG>* p_custom_pt_screen = nullptr
		) noexcept;
	};
}