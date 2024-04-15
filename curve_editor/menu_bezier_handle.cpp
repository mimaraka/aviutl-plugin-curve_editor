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

	void BezierHandleMenu::update_state(const BezierHandle* p_handle) noexcept {
		if (p_handle == nullptr) return;

		bool is_curve_adjacent = false;

		switch (p_handle->type()) {
		case BezierHandle::Type::Left:
			is_curve_adjacent = p_handle->p_curve()->prev() != nullptr;
			break;

		case BezierHandle::Type::Right:
			is_curve_adjacent = p_handle->p_curve()->next() != nullptr;
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

	HMENU BezierHandleMenu::get_handle(const BezierHandle* p_handle) noexcept {
		update_state(p_handle);
		return Menu::get_handle();
	}

	int BezierHandleMenu::show(
		BezierHandle* p_handle,
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
		update_state(p_handle);
		int ret = ::TrackPopupMenu(
			menu_,
			flags | TPM_RETURNCMD | TPM_NONOTIFY,
			tmp.x,
			tmp.y,
			0, hwnd, NULL
		);
		return callback(p_handle, view, ret);
	}

	bool BezierHandleMenu::callback(BezierHandle* p_handle, const GraphView& view, uint16_t id) noexcept {
		using StringId = global::StringTable::StringId;
		switch (id) {
		case ID_BEZIER_HANDLE_ROTATE:
			p_handle->adjust_angle(view);
			global::window_grapheditor.redraw();
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