#include "config.hpp"
#include "curve_editor.hpp"
#include "dialog_modifier.hpp"
#include "enum.hpp"
#include "menu_curve_segment.hpp"
#include "resource.h"
#include "string_table.hpp"



namespace cved {
	CurveSegmentMenu::CurveSegmentMenu(HINSTANCE hinst) {
		using StringId = global::StringTable::StringId;
		constexpr size_t n_segment_types = (size_t)CurveSegmentType::NumCurveSegmentType;
		// カーブセグメントの名前の配列
		const char* const segment_type_names[n_segment_types] = {
			global::string_table[StringId::LabelCurveSegmentTypeLinear],
			global::string_table[StringId::LabelEditModeBezier],
			global::string_table[StringId::LabelEditModeElastic],
			global::string_table[StringId::LabelEditModeBounce]
		};

		// カーブタイプのサブメニューの作成
		menu_ = ::GetSubMenu(::LoadMenuA(hinst, MAKEINTRESOURCEA(IDR_MENU_CURVE)), 0);
		submenu_segment_type_ = ::CreatePopupMenu();
		MENUITEMINFOA minfo_tmp;
		minfo_tmp.cbSize = sizeof(MENUITEMINFOA);
		minfo_tmp.fMask = MIIM_TYPE | MIIM_ID;

		for (uint32_t i = 0u; i < n_segment_types; i++) {
			minfo_tmp.fType = MFT_RADIOCHECK;
			minfo_tmp.wID = (WORD)WindowCommand::CurveSegmentTypeLinear + i;
			minfo_tmp.dwTypeData = const_cast<char*>(segment_type_names[i]);
			::InsertMenuItemA(submenu_segment_type_, i, TRUE, &minfo_tmp);
		}

		// カーブタイプのサブメニューの設定
		minfo_tmp.fMask = MIIM_SUBMENU;
		minfo_tmp.hSubMenu = submenu_segment_type_;
		::SetMenuItemInfoA(menu_, ID_CURVE_TYPE, FALSE, &minfo_tmp);
	}

	void CurveSegmentMenu::update_state(uint32_t curve_id) noexcept {
		constexpr size_t n_segment_types = (size_t)CurveSegmentType::NumCurveSegmentType;
		// カーブセグメントの型情報(のポインタ)の配列
		const std::type_info* p_segment_types[n_segment_types] = {
			&typeid(LinearCurve),
			&typeid(BezierCurve),
			&typeid(ElasticCurve),
			&typeid(BounceCurve)
		};

		auto curve = global::id_manager.get_curve<GraphCurve>(curve_id);
		if (!curve) {
			return;
		}
		for (size_t i = 0; i < n_segment_types; i++) {
			MENUITEMINFOA minfo_tmp;
			minfo_tmp.cbSize = sizeof(MENUITEMINFOA);
			minfo_tmp.fMask = MIIM_STATE;
			minfo_tmp.fState = typeid(*curve) == *p_segment_types[i] ? MFS_CHECKED : MFS_UNCHECKED;
			::SetMenuItemInfoA(submenu_segment_type_, i, TRUE, &minfo_tmp);
		}


		// 貼り付けるカーブのデータがないときは"貼り付け"のメニューを無効にする
		MENUITEMINFOA minfo_tmp;
		minfo_tmp.cbSize = sizeof(MENUITEMINFOA);
		minfo_tmp.fMask = MIIM_STATE;
		minfo_tmp.fState = global::editor.editor_graph().is_copying() ? MFS_ENABLED : MFS_DISABLED;
		::SetMenuItemInfoA(menu_, ID_CURVE_PASTE, FALSE, &minfo_tmp);
	}

	HMENU CurveSegmentMenu::get_handle(uint32_t curve_id) noexcept {
		update_state(curve_id);
		return Menu::get_handle();
	}

	int CurveSegmentMenu::show(
		uint32_t curve_id,
		const MyWebView2& webview,
		HWND hwnd,
		UINT flags,
		const mkaul::Point<LONG>* p_custom_pt_screen
	) noexcept {
		POINT tmp;
		::GetCursorPos(&tmp);
		if (p_custom_pt_screen) {
			tmp = p_custom_pt_screen->to<POINT>();
		}
		update_state(curve_id);
		auto ret = ::TrackPopupMenu(
			menu_,
			flags | TPM_RETURNCMD | TPM_NONOTIFY,
			tmp.x,
			tmp.y,
			0, hwnd, NULL
		);
		return callback(curve_id, webview, static_cast<uint16_t>(ret));
	}

	bool CurveSegmentMenu::callback(uint32_t curve_id, const MyWebView2& webview, uint16_t id) noexcept {
		if (mkaul::in_range(
			id,
			(uint16_t)WindowCommand::CurveSegmentTypeLinear,
			(uint16_t)WindowCommand::CurveSegmentTypeLinear + (uint16_t)CurveSegmentType::NumCurveSegmentType - 1u
		)) {
			global::editor.editor_graph().curve_normal()->replace_curve(
				curve_id,
				(CurveSegmentType)(id - (uint16_t)WindowCommand::CurveSegmentTypeLinear)
			);
			webview.post_message(L"editor-graph", L"updateHandles");
			return true;
		}

		switch (id) {
		case ID_CURVE_COPY:
			break;

		case ID_CURVE_PASTE:
			break;

		case ID_CURVE_REVERSE:
		{
			auto p_curve_normal = global::editor.editor_graph().curve_normal();
			if (p_curve_normal) {
				p_curve_normal->reverse_segment(curve_id);
				webview.post_message(L"editor-graph", L"updateHandlePos");
			}
			break;
		}

		case ID_CURVE_MODIFIER:
		{
			auto p_curve_normal = global::editor.editor_graph().curve_normal();
			if (p_curve_normal) {
				ModifierDialog dialog;
				dialog.show(webview.get_hwnd(), static_cast<LPARAM>(curve_id));
			}
			break;
		}

		default:
			return false;
		}

		return true;
	}
}