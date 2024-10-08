#include "menu_bezier_handle.hpp"
#include "curve_editor.hpp"
#include "global.hpp"
#include "my_messagebox.hpp"
#include "string_table.hpp"
#include "resource.h"



namespace cved {
	BezierHandleMenu::BezierHandleMenu(HINSTANCE hinst) {
		menu_ = ::GetSubMenu(::LoadMenuA(hinst, MAKEINTRESOURCEA(IDR_MENU_BEZIER_HANDLE)), 0);
	}

	void BezierHandleMenu::update_state(size_t idx, BezierHandle::Type handle_type) noexcept {
		auto curve_normal = global::editor.editor_graph().curve_normal();
		if (!curve_normal->check_segment_type<BezierCurve>(idx)) return;

		bool is_curve_adjacent = false;

		switch (handle_type) {
		case BezierHandle::Type::Left:
			is_curve_adjacent = curve_normal->get_segment(idx)->prev() != nullptr;
			break;

		case BezierHandle::Type::Right:
			is_curve_adjacent = curve_normal->get_segment(idx)->next() != nullptr;
			break;

		default:
			return;
		}

		MENUITEMINFOA minfo_tmp;
		minfo_tmp.cbSize = sizeof(MENUITEMINFOA);
		minfo_tmp.fMask = MIIM_STATE;
		minfo_tmp.fState = is_curve_adjacent ? MFS_ENABLED : MFS_DISABLED;
		::SetMenuItemInfoA(menu_, ID_BEZIER_HANDLE_ROTATE, FALSE, &minfo_tmp);
	}

	HMENU BezierHandleMenu::get_handle(size_t idx, BezierHandle::Type handle_type) noexcept {
		update_state(idx, handle_type);
		return Menu::get_handle();
	}

	int BezierHandleMenu::show(
		size_t idx,
		BezierHandle::Type handle_type,
		const GraphView& view,
		HWND hwnd,
		UINT flags,
		const mkaul::Point<LONG>* p_custom_pt_screen
	) noexcept {
		POINT tmp;
		::GetCursorPos(&tmp);
		if (p_custom_pt_screen) {
			tmp = p_custom_pt_screen->to<POINT>();
		}
		update_state(idx, handle_type);
		int ret = ::TrackPopupMenu(
			menu_,
			flags | TPM_RETURNCMD | TPM_NONOTIFY,
			tmp.x,
			tmp.y,
			0, hwnd, NULL
		);
		return callback(idx, handle_type, view, (int16_t)ret);
	}

	bool BezierHandleMenu::callback(size_t idx, BezierHandle::Type handle_type, const GraphView& view, uint16_t id) noexcept {
		using StringId = global::StringTable::StringId;
		switch (id) {
		case ID_BEZIER_HANDLE_ROTATE:
			//if (global::editor.editor_graph().curve_normal()->adjust_segment_handle_angle(idx, handle_type, view)) {
			//	//global::window_grapheditor.redraw();
			//}
			break;

		case ID_BEZIER_HANDLE_ROOT:
			// TODO: 「ハンドルを始点に移動」の実装
			break;

		default:
			return false;
		}
		return true;
	}
}