#include "menu_curve_segment.hpp"
#include "config.hpp"
#include "curve_editor.hpp"
#include "dialog_modifier.hpp"
#include "enum.hpp"
#include "global.hpp"
#include "string_table.hpp"
#include "resource.h"



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
	
	std::optional<uint32_t> CurveSegmentMenu::get_idx(GraphCurve* p_segment) noexcept {
		auto p_curve_normal = global::editor.editor_graph().curve_normal();
		if (p_curve_normal) {
			for (size_t i = 0; i < p_curve_normal->segment_n(); i++) {
				if (p_curve_normal->get_segment(i) == p_segment) {
					return i;
				}
			}
		}
		return std::nullopt;
	}

	void CurveSegmentMenu::update_state(uint32_t idx) noexcept {
		constexpr size_t n_segment_types = (size_t)CurveSegmentType::NumCurveSegmentType;
		// カーブセグメントの型情報(のポインタ)の配列
		const std::type_info* p_segment_types[n_segment_types] = {
			&typeid(LinearCurve),
			&typeid(BezierCurve),
			&typeid(ElasticCurve),
			&typeid(BounceCurve)
		};

		// TODO: 値指定モードに対応させる
		if (global::config.get_edit_mode() == EditMode::Normal) {
			auto p_curve_normal = global::editor.editor_graph().curve_normal();
			if (p_curve_normal and idx < p_curve_normal->segment_n()) {
				// カーブタイプのサブメニューのチェック状態を更新
				for (uint32_t i = 0u; i < n_segment_types; i++) {
					MENUITEMINFOA minfo_tmp;
					minfo_tmp.cbSize = sizeof(MENUITEMINFOA);
					minfo_tmp.fMask = MIIM_STATE;
					minfo_tmp.fState = typeid(*(p_curve_normal->get_segment(idx))) == *p_segment_types[i] ? MFS_CHECKED : MFS_UNCHECKED;
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

	HMENU CurveSegmentMenu::get_handle(uint32_t idx) noexcept {
		update_state(idx);
		return Menu::get_handle();
	}

	int CurveSegmentMenu::show(
		GraphCurve* p_segment,
		HWND hwnd,
		UINT flags,
		const mkaul::Point<LONG>* p_custom_pt_screen
	) noexcept {
		auto idx = get_idx(p_segment);
		if (!idx.has_value()) {
			return 0;
		}
		POINT tmp;
		::GetCursorPos(&tmp);
		if (p_custom_pt_screen) {
			tmp = p_custom_pt_screen->to<POINT>();
		}
		update_state(idx.value());
		auto ret = ::TrackPopupMenu(
			menu_,
			flags | TPM_RETURNCMD | TPM_NONOTIFY,
			tmp.x,
			tmp.y,
			0, hwnd, NULL
		);
		return callback(idx.value(), static_cast<uint16_t>(ret));
	}

	bool CurveSegmentMenu::callback(uint32_t idx, uint16_t id) noexcept {
		if (mkaul::in_range(
			id,
			(uint16_t)WindowCommand::CurveSegmentTypeLinear,
			(uint16_t)WindowCommand::CurveSegmentTypeLinear + (uint16_t)CurveSegmentType::NumCurveSegmentType - 1u,
			true
		)) {
			global::editor.editor_graph().curve_normal()->replace_curve(
				idx,
				(CurveSegmentType)(id - (uint16_t)WindowCommand::CurveSegmentTypeLinear)
			);
			//global::window_grapheditor.redraw();
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
				p_curve_normal->reverse_segment(idx);
				//global::window_grapheditor.redraw();
			}
			break;
		}

		case ID_CURVE_MODIFIER:
		{
			auto p_curve_normal = global::editor.editor_graph().curve_normal();
			if (p_curve_normal) {
				ModifierDialog dialog;
				dialog.show(global::fp->hwnd, reinterpret_cast<LPARAM>(p_curve_normal->get_segment(idx)));
			}
			break;
		}

		default:
			return false;
		}

		return true;
	}
}