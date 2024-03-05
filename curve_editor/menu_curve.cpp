#include "menu_curve.hpp"
#include "config.hpp"
#include "curve_editor.hpp"
#include "enum.hpp"
#include "string_table.hpp"
#include "resource.h"



namespace cved {
	CurveMenu::CurveMenu(HINSTANCE hinst) {
		using StringId = global::StringTable::StringId;
		constexpr size_t n_segment_types = (size_t)CurveSegmentType::NumCurveSegmentType;
		// カーブセグメントの名前の配列
		const char* const segment_type_names[n_segment_types] = {
			global::string_table[StringId::LabelCurveSegmentTypeLinear],
			global::string_table[StringId::LabelEditModeBezier],
			global::string_table[StringId::LabelEditModeElastic],
			global::string_table[StringId::LabelEditModeBounce],
			global::string_table[StringId::LabelEditModeStep]
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

	void CurveMenu::update_state(size_t index) noexcept {
		constexpr size_t n_segment_types = (size_t)CurveSegmentType::NumCurveSegmentType;
		// カーブセグメントの型情報(のポインタ)の配列
		const std::type_info* p_segment_types[n_segment_types] = {
			&typeid(LinearCurve),
			&typeid(BezierCurve),
			&typeid(ElasticCurve),
			&typeid(BounceCurve),
			&typeid(StepCurve)
		};

		// TODO: 値指定モードに対応させる
		if (global::config.get_edit_mode() == EditMode::Normal) {
			auto p_curve = reinterpret_cast<NormalCurve*>(global::editor.editor_graph().current_curve());
			if (index < p_curve->get_curve_segments().size()) {
				// カーブタイプのサブメニューのチェック状態を更新
				for (uint32_t i = 0u; i < n_segment_types; i++) {
					MENUITEMINFOA minfo_tmp;
					minfo_tmp.cbSize = sizeof(MENUITEMINFOA);
					minfo_tmp.fMask = MIIM_STATE;
					minfo_tmp.fState = typeid(*(p_curve->get_curve_segments()[index])) == *p_segment_types[i] ? MFS_CHECKED : MFS_UNCHECKED;
					::SetMenuItemInfoA(submenu_segment_type_, i, TRUE, &minfo_tmp);
				}
			}
		}

		// 貼り付けるカーブのデータがないときは"貼り付け"のメニューを無効にする
		MENUITEMINFOA minfo_tmp;
		minfo_tmp.cbSize = sizeof(MENUITEMINFOA);
		minfo_tmp.fMask = MIIM_STATE;
		minfo_tmp.fState = global::editor.editor_graph().is_copying() ? MFS_ENABLED : MFS_DISABLED;
		::SetMenuItemInfoA(menu_, ID_CURVE_PASTE, FALSE, &minfo_tmp);
	}

	HMENU CurveMenu::get_handle(size_t index) noexcept {
		update_state(index);
		return Menu::get_handle();
	}

	int CurveMenu::show(
		size_t index,
		HWND hwnd,
		UINT flags,
		const mkaul::Point<LONG>* p_custom_point_screen
	) noexcept {
		update_state(index);
		return Menu::show(hwnd, flags, p_custom_point_screen);
	}

	bool CurveMenu::callback(WPARAM wparam, LPARAM lparam) noexcept {
		return false;
	}
}