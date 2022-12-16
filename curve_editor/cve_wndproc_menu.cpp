//----------------------------------------------------------------------------------
//		Curve Editor
//		ソースファイル(ウィンドウプロシージャ)
//		Visual C++ 2022
//----------------------------------------------------------------------------------

#include "cve_header.hpp"



//---------------------------------------------------------------------
//		ウィンドウプロシージャ（上部メニュー）
//---------------------------------------------------------------------
LRESULT CALLBACK wndproc_menu(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	RECT rect_wnd;

	static HMENU menu;
	static MENUITEMINFO minfo;

	static cve::Bitmap_Buffer bitmap_buffer;
	static cve::Button	copy,
						read,
						save,
						clear,
						fit,
						mode,
						id_back,
						id_next;

	static cve::Button_Param param;
	static cve::Button_ID id_id;

	cve::Rectangle		rect_lower_buttons,
						rect_id_buttons;

	RECT				rect_copy,
						rect_read,
						rect_save,
						rect_clear,
						rect_fit,
						rect_mode,
						rect_id_back,
						rect_id_id,
						rect_id_next;

	POINT			pt_client = {
		GET_X_LPARAM(lparam),
		GET_Y_LPARAM(lparam)
	};

	LPCTSTR str_mode[] = {
		CVE_STR_MODE_BEZIER,
		CVE_STR_MODE_MULTIBEZIER,
		CVE_STR_MODE_VALUE,
		CVE_STR_MODE_ELASTIC,
		CVE_STR_MODE_BOUNCE
	};


	::GetClientRect(hwnd, &rect_wnd);

	// 下部のボタンが並んだRECT
	rect_lower_buttons.set(
		CVE_MARGIN,
		CVE_MARGIN + CVE_MARGIN_CONTROL + CVE_CT_UPPER_H,
		rect_wnd.right - CVE_MARGIN,
		CVE_MARGIN + CVE_MARGIN_CONTROL + CVE_CT_LOWER_H + CVE_CT_UPPER_H
	);

	// モード選択ボタンのRECT
	rect_mode = {
		CVE_MARGIN,
		CVE_MARGIN,
		(rect_wnd.right - CVE_MARGIN) / 2,
		CVE_MARGIN + CVE_CT_UPPER_H
	};

	// IDのボタンが並んだRECT
	rect_id_buttons.set(
		(rect_wnd.right + CVE_MARGIN) / 2,
		CVE_MARGIN,
		rect_wnd.right - CVE_MARGIN,
		CVE_MARGIN + CVE_CT_UPPER_H
	);

	LPRECT lower_buttons[] = {
		&rect_copy,
		&rect_read,
		&rect_save,
		&rect_clear,
		&rect_fit
	};

	LPRECT id_buttons[] = {
		&rect_id_back,
		&rect_id_id,
		&rect_id_next
	};

	rect_lower_buttons.divide(lower_buttons, NULL, 5);
	rect_id_buttons.divide(id_buttons, NULL, 3);

	switch (msg) {
	case WM_CLOSE:
		bitmap_buffer.exit();
		return 0;

	case WM_CREATE:
		bitmap_buffer.init(hwnd);
		bitmap_buffer.set_size(rect_wnd);

		// メニュー
		menu = ::GetSubMenu(LoadMenu(g_fp->dll_hinst, MAKEINTRESOURCE(IDR_MENU_MODE)), 0);
		minfo.cbSize = sizeof(MENUITEMINFO);

		// モード選択ボタン
		mode.initialize(
			hwnd,
			"CTRL_MODE",
			NULL,
			cve::Button::String,
			NULL, NULL,
			str_mode[g_config.edit_mode],
			CVE_CT_EDIT_MODE,
			rect_mode,
			CVE_EDGE_ALL
		);

		// 値
		param.initialize(
			hwnd,
			"CTRL_VALUE",
			NULL,
			cve::Button::String,
			NULL, NULL,
			NULL,
			CVE_CM_PARAM,
			rect_id_buttons.rect,
			CVE_EDGE_ALL
		);

		// 前のIDのカーブに移動
		id_back.initialize(
			hwnd,
			"CTRL_ID_BACK",
			"前のIDのカーブに移動",
			cve::Button::Icon,
			MAKEINTRESOURCE(IDI_BACK),
			MAKEINTRESOURCE(IDI_BACK_LIGHT),
			NULL,
			CVE_CM_ID_BACK,
			rect_id_back,
			CVE_EDGE_ALL
		);

		// ID表示ボタン
		id_id.initialize(
			hwnd,
			"CTRL_ID_ID",
			NULL,
			cve::Button::String,
			NULL, NULL,
			NULL,
			NULL,
			rect_id_id,
			CVE_EDGE_ALL
		);

		// 次のIDのカーブに移動
		id_next.initialize(
			hwnd,
			"CTRL_ID_NEXT",
			"次のIDのカーブに移動",
			cve::Button::Icon,
			MAKEINTRESOURCE(IDI_NEXT),
			MAKEINTRESOURCE(IDI_NEXT_LIGHT),
			NULL,
			CVE_CM_ID_NEXT,
			rect_id_next,
			CVE_EDGE_ALL
		);

		// コピー
		copy.initialize(
			hwnd,
			"CTRL_COPY",
			"カーブの値をコピー",
			cve::Button::Icon,
			MAKEINTRESOURCE(IDI_COPY),
			MAKEINTRESOURCE(IDI_COPY_LIGHT),
			NULL,
			CVE_CM_COPY,
			rect_copy,
			CVE_EDGE_ALL
		);

		// 読み取り
		read.initialize(
			hwnd,
			"CTRL_READ",
			"カーブの値を読み取り",
			cve::Button::Icon,
			MAKEINTRESOURCE(IDI_READ),
			MAKEINTRESOURCE(IDI_READ_LIGHT),
			NULL,
			CVE_CM_READ,
			rect_read,
			CVE_EDGE_ALL
		);

		// 保存
		save.initialize(
			hwnd,
			"CTRL_SAVE",
			"カーブをプリセットとして保存",
			cve::Button::Icon,
			MAKEINTRESOURCE(IDI_SAVE),
			MAKEINTRESOURCE(IDI_SAVE_LIGHT),
			NULL,
			CVE_CM_SAVE,
			rect_save,
			CVE_EDGE_ALL
		);

		// 初期化
		clear.initialize(
			hwnd,
			"CTRL_CLEAR",
			"カーブを初期化",
			cve::Button::Icon,
			MAKEINTRESOURCE(IDI_CLEAR),
			MAKEINTRESOURCE(IDI_CLEAR_LIGHT),
			NULL,
			CVE_CM_CLEAR,
			rect_clear,
			CVE_EDGE_ALL
		);

		// フィット
		fit.initialize(
			hwnd,
			"CTRL_FIT",
			"グラフをフィット",
			cve::Button::Icon,
			MAKEINTRESOURCE(IDI_FIT),
			MAKEINTRESOURCE(IDI_FIT_LIGHT),
			NULL,
			CVE_CM_FIT,
			rect_fit,
			CVE_EDGE_ALL
		);

		switch (g_config.edit_mode) {
		case cve::Mode_Bezier:
			id_id.hide();
			id_back.hide();
			id_next.hide();
			break;

		case cve::Mode_Multibezier:
			param.hide();
			copy.set_status(CVE_BUTTON_DISABLED);
			break;

		case cve::Mode_Value:
			param.hide();
			copy.set_status(CVE_BUTTON_DISABLED);
			read.set_status(CVE_BUTTON_DISABLED);
			save.set_status(CVE_BUTTON_DISABLED);
			break;

		case cve::Mode_Elastic:
			id_id.hide();
			id_back.hide();
			id_next.hide();
			break;

		case cve::Mode_Bounce:
			id_id.hide();
			id_back.hide();
			id_next.hide();
			break;
		}

		return 0;

	case WM_SIZE:
		bitmap_buffer.set_size(rect_wnd);

		mode.move(rect_mode);

		param.move(rect_id_buttons.rect);

		id_back.move(rect_id_back);
		id_id.move(rect_id_id);
		id_next.move(rect_id_next);

		copy.move(rect_copy);
		read.move(rect_read);
		save.move(rect_save);
		clear.move(rect_clear);
		fit.move(rect_fit);
		return 0;

	case WM_PAINT:
		bitmap_buffer.draw_panel();
		return 0;

	case WM_COMMAND:
		switch (wparam) {
		case CVE_CM_REDRAW:
			::InvalidateRect(hwnd, NULL, FALSE);
			mode.redraw();

			param.redraw();

			id_back.redraw();
			id_id.redraw();
			id_next.redraw();

			copy.redraw();
			read.redraw();
			save.redraw();
			clear.redraw();
			fit.redraw();
			return 0;

			// 編集モード選択ボタン
		case CVE_CT_EDIT_MODE:
		{
			minfo.fMask = MIIM_STATE;

			const int edit_mode_menu_id[] = {
				ID_MODE_BEZIER,
				ID_MODE_MULTIBEZIER,
				ID_MODE_VALUE,
				ID_MODE_ELASTIC,
				ID_MODE_BOUNCE,
			};

			// モードのチェック
			for (int i = 0; i < sizeof(edit_mode_menu_id) / sizeof(int); i++) {
				minfo.fState = g_config.edit_mode == i ? MFS_CHECKED : MFS_UNCHECKED;
				SetMenuItemInfo(menu, edit_mode_menu_id[i], FALSE, &minfo);
			}

			::ClientToScreen(hwnd, &pt_client);
			::TrackPopupMenu(menu, TPM_LEFTALIGN | TPM_TOPALIGN, pt_client.x, pt_client.y, 0, hwnd, NULL);

			return 0;
		}

		// ベジェモード
		case CVE_CT_EDIT_MODE_NORMAL:
		case ID_MODE_BEZIER:
			g_config.edit_mode = cve::Mode_Bezier;
			::SendMessage(mode.hwnd, WM_COMMAND, CVE_CM_CHANGE_LABEL, (LPARAM)str_mode[g_config.edit_mode]);
			param.show();
			id_id.hide();
			id_back.hide();
			id_next.hide();
			copy.set_status(CVE_BUTTON_ENABLED);
			read.set_status(CVE_BUTTON_ENABLED);
			save.set_status(CVE_BUTTON_ENABLED);
			param.redraw();
			g_window_editor.redraw();

			return 0;

		// マルチベジェモード
		case CVE_CT_EDIT_MODE_MULTIBEZIER:
		case ID_MODE_MULTIBEZIER:
			g_config.edit_mode = cve::Mode_Multibezier;
			::SendMessage(mode.hwnd, WM_COMMAND, CVE_CM_CHANGE_LABEL, (LPARAM)str_mode[g_config.edit_mode]);
			param.hide();
			id_id.show();
			id_back.show();
			id_next.show();
			copy.set_status(CVE_BUTTON_DISABLED);
			read.set_status(CVE_BUTTON_ENABLED);
			save.set_status(CVE_BUTTON_ENABLED);
			g_window_editor.redraw();

			return 0;

		// 数値指定モード
		case CVE_CT_EDIT_MODE_VALUE:
		case ID_MODE_VALUE:
			g_config.edit_mode = cve::Mode_Value;
			::SendMessage(mode.hwnd, WM_COMMAND, CVE_CM_CHANGE_LABEL, (LPARAM)str_mode[g_config.edit_mode]);
			param.hide();
			id_id.show();
			id_back.show();
			id_next.show();
			copy.set_status(CVE_BUTTON_DISABLED);
			read.set_status(CVE_BUTTON_DISABLED);
			save.set_status(CVE_BUTTON_DISABLED);
			g_window_editor.redraw();

			return 0;

		// 振動モード
		case CVE_CT_EDIT_MODE_ELASTIC:
		case ID_MODE_ELASTIC:
			g_config.edit_mode = cve::Mode_Elastic;
			::SendMessage(mode.hwnd, WM_COMMAND, CVE_CM_CHANGE_LABEL, (LPARAM)str_mode[g_config.edit_mode]);
			param.show();
			id_id.hide();
			id_back.hide();
			id_next.hide();
			copy.set_status(CVE_BUTTON_ENABLED);
			read.set_status(CVE_BUTTON_ENABLED);
			save.set_status(CVE_BUTTON_ENABLED);
			param.redraw();
			g_window_editor.redraw();

			return 0;

		// 弾性モード
		case CVE_CT_EDIT_MODE_BOUNCE:
		case ID_MODE_BOUNCE:
			g_config.edit_mode = cve::Mode_Bounce;
			::SendMessage(mode.hwnd, WM_COMMAND, CVE_CM_CHANGE_LABEL, (LPARAM)str_mode[g_config.edit_mode]);
			param.show();
			id_id.hide();
			id_back.hide();
			id_next.hide();
			copy.set_status(CVE_BUTTON_ENABLED);
			read.set_status(CVE_BUTTON_ENABLED);
			save.set_status(CVE_BUTTON_ENABLED);
			param.redraw();
			g_window_editor.redraw();

			return 0;

			// 前のIDのカーブに移動
		case CVE_CM_ID_BACK:
			if (g_config.edit_mode == cve::Mode_Multibezier) {
				g_config.current_id.multibezier--;
				g_config.current_id.multibezier = MINMAX_LIMIT(g_config.current_id.multibezier, 1, CVE_CURVE_MAX);
				g_curve_mb_previous = g_curve_mb[g_config.current_id.multibezier - 1];
				id_id.redraw();
				g_window_editor.redraw();
			}
			else if (g_config.edit_mode == cve::Mode_Value) {
				g_config.current_id.value--;
				g_config.current_id.value = MINMAX_LIMIT(g_config.current_id.value, 1, CVE_CURVE_MAX);
				g_curve_value_previous = g_curve_value[g_config.current_id.value - 1];
			}
			id_id.redraw();
			g_window_editor.redraw();
			return 0;

			// 次のIDのカーブに移動
		case CVE_CM_ID_NEXT:
			if (g_config.edit_mode == cve::Mode_Multibezier) {
				g_config.current_id.multibezier++;
				g_config.current_id.multibezier = MINMAX_LIMIT(g_config.current_id.multibezier, 1, CVE_CURVE_MAX);
				g_curve_mb_previous = g_curve_mb[g_config.current_id.multibezier - 1];
				id_id.redraw();
				g_window_editor.redraw();
			}
			else if (g_config.edit_mode == cve::Mode_Value) {
				g_config.current_id.value++;
				g_config.current_id.value = MINMAX_LIMIT(g_config.current_id.value, 1, CVE_CURVE_MAX);
				g_curve_value_previous = g_curve_value[g_config.current_id.value - 1];
			}
			id_id.redraw();
			g_window_editor.redraw();
			return 0;

			// IDを変更
		case CVE_CM_CHANGE_ID:
			if (g_config.edit_mode == cve::Mode_Multibezier) {
				g_config.current_id.multibezier = MINMAX_LIMIT(lparam, 1, CVE_CURVE_MAX);
				g_curve_mb_previous = g_curve_mb[g_config.current_id.multibezier - 1];
			}
			else if (g_config.edit_mode == cve::Mode_Multibezier) {
				g_config.current_id.value = MINMAX_LIMIT(lparam, 1, CVE_CURVE_MAX);
				g_curve_value_previous = g_curve_value[g_config.current_id.value - 1];
			}
			id_id.redraw();
			g_window_editor.redraw();
			return 0;

		case CVE_CM_PARAM_REDRAW:
			param.redraw();
			return 0;

		case CVE_CM_PARAM:
			::DialogBox(g_fp->dll_hinst, MAKEINTRESOURCE(IDD_PARAM), hwnd, dialogproc_param_normal);
			::SendMessage(g_window_editor.hwnd, WM_COMMAND, CVE_CM_REDRAW, 0);
			return 0;

		case CVE_CM_COPY:
			::SendMessage(g_window_editor.hwnd, WM_COMMAND, CVE_CM_COPY, 0);
			return 0;

		case CVE_CM_READ:
			::SendMessage(g_window_editor.hwnd, WM_COMMAND, CVE_CM_READ, 0);
			return 0;

		case CVE_CM_SAVE:
			::SendMessage(g_window_editor.hwnd, WM_COMMAND, CVE_CM_SAVE, 0);
			return 0;

		case CVE_CM_CLEAR:
			::SendMessage(g_window_editor.hwnd, WM_COMMAND, CVE_CM_CLEAR, 0);
			return 0;

		case CVE_CM_FIT:
			::SendMessage(g_window_editor.hwnd, WM_COMMAND, CVE_CM_FIT, 0);
			return 0;
		}
		return 0;

	default:
		return ::DefWindowProc(hwnd, msg, wparam, lparam);
	}
	return 0;
}