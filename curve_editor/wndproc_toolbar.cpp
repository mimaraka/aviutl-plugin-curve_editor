#include "wndproc_toolbar.hpp"
#include <mkaul/include/graphics.hpp>
#include <mkaul/include/ui.hpp>
#include "config.hpp"
#include "constants.hpp"
#include "curve_editor.hpp"
#include "dialog_curve_code.hpp"
#include "global.hpp"
#include "my_messagebox.hpp"
#include "menu_edit_mode.hpp"
#include "menu_more.hpp"
#include "string_table.hpp"
#include "util.hpp"
#include "resource.h"



namespace cved {
	LRESULT CALLBACK wndproc_toolbar(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam) {
		using namespace mkaul::graphics;
		using namespace mkaul::ui;
		using StringId = global::StringTable::StringId;
		using RoundEdge = mkaul::flag::RoundEdge;

		constexpr int BUTTON_ID_MODE = 0x0001;
		constexpr int BUTTON_ID_PARAM = 0x0002;
		constexpr int BUTTON_ID_COPY = 0x0003;
		constexpr int BUTTON_ID_READ = 0x0004;
		constexpr int BUTTON_ID_SAVE = 0x0005;
		constexpr int BUTTON_ID_CLEAR = 0x0006;
		constexpr int BUTTON_ID_FIT = 0x0007;
		constexpr int BUTTON_ID_MORE = 0x0008;
		constexpr int BUTTON_ID_ID = 0x0009;
		constexpr int BUTTON_ID_BACK = 0x000a;
		constexpr int BUTTON_ID_NEXT = 0x000b;
		constexpr int PADDING_CONTROL_WIDE = 4;
		constexpr int PADDING_CONTROL_NARROW = 2;

		static std::unique_ptr<Graphics> p_graphics = Factory::create_graphics();

		static IconButton button_copy, button_read, button_save, button_clear, button_fit, button_more, button_id_back, button_id_next;
		static LabelButton button_mode, button_param, button_id;
		static Row row_id{ &button_id_back, &button_id, &button_id_next };
		static Row row_top{ &button_copy, &button_read, &button_save, &button_clear, &button_fit, &button_more };
		static Row row_bottom{ &button_mode, &button_param };
		static Column toolbar{ &row_top, &row_bottom };

		static EditModeMenu menu_edit_mode;
		static MoreMenu menu_more{ global::fp->dll_hinst };

		RECT rect_wnd;
		::GetClientRect(hwnd, &rect_wnd);

		switch (message) {
		case WM_CREATE:
		{
			if (!p_graphics->init(hwnd)) {
				my_messagebox(global::string_table[StringId::ErrorGraphicsInitFailed], hwnd, MessageBoxIcon::Error);
				::DestroyWindow(hwnd);
			}

			mkaul::graphics::Font font{ 10.f, "Yu Gothic UI Semibold", mkaul::flag::FontStyle::Regular, 600 };

			// ボタン(コピー)
			button_copy.create(
				global::fp->dll_hinst,
				hwnd,
				BUTTON_ID_COPY,
				&global::config.get_theme().bg,
				&global::config.get_theme().bg,
				global::string_table[StringId::LabelTooltipCopy],
				mkaul::WindowRectangle{},
				mkaul::WindowRectangle{
					PADDING_CONTROL_WIDE,
					PADDING_CONTROL_WIDE,
					PADDING_CONTROL_NARROW,
					PADDING_CONTROL_NARROW
				},
				RoundEdge::All,
				global::ROUND_RADIUS
			);

			// アイコン(コピー)
			button_copy.set_icon_from_resource(
				global::fp->dll_hinst,
				MAKEINTRESOURCEA(IDB_COPY),
				"PNG"
			);

			// ボタン(カーブ読み取り)
			button_read.create(
				global::fp->dll_hinst,
				hwnd,
				BUTTON_ID_READ,
				&global::config.get_theme().bg,
				&global::config.get_theme().bg,
				global::string_table[StringId::LabelTooltipRead],
				mkaul::WindowRectangle{},
				mkaul::WindowRectangle{
					PADDING_CONTROL_NARROW,
					PADDING_CONTROL_WIDE,
					PADDING_CONTROL_NARROW,
					PADDING_CONTROL_NARROW
				},
				RoundEdge::All,
				global::ROUND_RADIUS
			);

			// アイコン(カーブ読み取り)
			button_read.set_icon_from_resource(
				global::fp->dll_hinst,
				MAKEINTRESOURCEA(IDB_READ),
				"PNG"
			);

			button_save.create(
				global::fp->dll_hinst,
				hwnd,
				BUTTON_ID_SAVE,
				&global::config.get_theme().bg,
				&global::config.get_theme().bg,
				global::string_table[StringId::LabelTooltipSave],
				mkaul::WindowRectangle{},
				mkaul::WindowRectangle{
					PADDING_CONTROL_NARROW,
					PADDING_CONTROL_WIDE,
					PADDING_CONTROL_NARROW,
					PADDING_CONTROL_NARROW
				},
				RoundEdge::All,
				global::ROUND_RADIUS
			);

			// アイコン(保存)
			button_save.set_icon_from_resource(
				global::fp->dll_hinst,
				MAKEINTRESOURCEA(IDB_SAVE),
				"PNG"
			);

			button_clear.create(
				global::fp->dll_hinst,
				hwnd,
				BUTTON_ID_CLEAR,
				&global::config.get_theme().bg,
				&global::config.get_theme().bg,
				global::string_table[StringId::LabelTooltipClear],
				mkaul::WindowRectangle{},
				mkaul::WindowRectangle{
					PADDING_CONTROL_NARROW,
					PADDING_CONTROL_WIDE,
					PADDING_CONTROL_NARROW,
					PADDING_CONTROL_NARROW
				},
				RoundEdge::All,
				global::ROUND_RADIUS
			);

			button_clear.set_icon_from_resource(
				global::fp->dll_hinst,
				MAKEINTRESOURCEA(IDB_CLEAR),
				"PNG"
			);

			button_fit.create(
				global::fp->dll_hinst,
				hwnd,
				BUTTON_ID_FIT,
				&global::config.get_theme().bg,
				&global::config.get_theme().bg,
				global::string_table[StringId::LabelTooltipFit],
				mkaul::WindowRectangle{},
				mkaul::WindowRectangle{
					PADDING_CONTROL_NARROW,
					PADDING_CONTROL_WIDE,
					PADDING_CONTROL_NARROW,
					PADDING_CONTROL_NARROW
				},
				RoundEdge::All,
				global::ROUND_RADIUS
			);

			button_fit.set_icon_from_resource(
				global::fp->dll_hinst,
				MAKEINTRESOURCEA(IDB_FIT),
				"PNG"
			);

			// ボタン(もっと見る)
			button_more.create(
				global::fp->dll_hinst,
				hwnd,
				BUTTON_ID_MORE,
				&global::config.get_theme().bg,
				&global::config.get_theme().bg,
				global::string_table[StringId::LabelTooltipMore],
				mkaul::WindowRectangle{},
				mkaul::WindowRectangle{
					PADDING_CONTROL_NARROW,
					PADDING_CONTROL_WIDE,
					PADDING_CONTROL_WIDE,
					PADDING_CONTROL_NARROW
				},
				RoundEdge::All,
				global::ROUND_RADIUS
			);

			button_more.set_icon_from_resource(
				global::fp->dll_hinst,
				MAKEINTRESOURCEA(IDB_MORE),
				"PNG"
			);

			// ボタン(モード選択)
			button_mode.create(
				global::fp->dll_hinst,
				hwnd,
				BUTTON_ID_MODE,
				global::config.get_edit_mode_str(),
				font,
				&global::config.get_theme().bg,
				&global::config.get_theme().bg,
				&global::config.get_theme().button_label,
				global::string_table[StringId::WordEditMode],
				mkaul::WindowRectangle{},
				mkaul::WindowRectangle{
					PADDING_CONTROL_WIDE,
					PADDING_CONTROL_NARROW,
					PADDING_CONTROL_NARROW,
					PADDING_CONTROL_WIDE
				},
				RoundEdge::All,
				global::ROUND_RADIUS
			);

			// ボタン(パラメータ)
			button_param.create(
				global::fp->dll_hinst,
				hwnd,
				BUTTON_ID_PARAM,
				"203482384",
				font,
				&global::config.get_theme().bg,
				&global::config.get_theme().bg,
				&global::config.get_theme().button_label,
				"",
				mkaul::WindowRectangle{},
				mkaul::WindowRectangle{
					PADDING_CONTROL_NARROW,
					PADDING_CONTROL_NARROW,
					PADDING_CONTROL_WIDE,
					PADDING_CONTROL_WIDE
				},
				RoundEdge::All,
				global::ROUND_RADIUS
			);

			button_id.create(
				global::fp->dll_hinst,
				hwnd,
				BUTTON_ID_ID,
				"1",
				font,
				&global::config.get_theme().bg,
				&global::config.get_theme().bg,
				&global::config.get_theme().button_label,
				global::string_table[StringId::LabelTooltipCurrentId],
				mkaul::WindowRectangle{},
				mkaul::WindowRectangle{
					PADDING_CONTROL_NARROW,
					PADDING_CONTROL_NARROW,
					PADDING_CONTROL_NARROW,
					PADDING_CONTROL_WIDE
				},
				RoundEdge::All,
				global::ROUND_RADIUS
			);

			button_id_back.create(
				global::fp->dll_hinst,
				hwnd,
				BUTTON_ID_BACK,
				&global::config.get_theme().bg,
				&global::config.get_theme().bg,
				global::string_table[StringId::LabelTooltipIdBack],
				mkaul::WindowRectangle{},
				mkaul::WindowRectangle{
					PADDING_CONTROL_NARROW,
					PADDING_CONTROL_NARROW,
					PADDING_CONTROL_NARROW,
					PADDING_CONTROL_WIDE
				},
				RoundEdge::All,
				global::ROUND_RADIUS
			);

			button_id_back.set_icon_from_resource(
				global::fp->dll_hinst,
				MAKEINTRESOURCEA(IDB_BACK),
				"PNG"
			);

			button_id_next.create(
				global::fp->dll_hinst,
				hwnd,
				BUTTON_ID_NEXT,
				&global::config.get_theme().bg,
				&global::config.get_theme().bg,
				global::string_table[StringId::LabelTooltipIdNext],
				mkaul::WindowRectangle{},
				mkaul::WindowRectangle{
					PADDING_CONTROL_NARROW,
					PADDING_CONTROL_NARROW,
					PADDING_CONTROL_WIDE,
					PADDING_CONTROL_WIDE
				},
				RoundEdge::All,
				global::ROUND_RADIUS
			);

			button_id_next.set_icon_from_resource(
				global::fp->dll_hinst,
				MAKEINTRESOURCEA(IDB_NEXT),
				"PNG"
			);

			::SendMessage(hwnd, WM_COMMAND, (UINT)WindowCommand::Update, 0);

			return 0;
		}

		case WM_DESTROY:
		case WM_CLOSE:
			toolbar.close();
			p_graphics->release();
			return 0;

		case WM_SIZE:
			p_graphics->resize();
			toolbar.move(mkaul::WindowRectangle{ rect_wnd });
			return 0;

		case WM_PAINT:
			p_graphics->begin_draw();
			p_graphics->fill_background(global::config.get_theme().bg);
			p_graphics->end_draw();
			return 0;

		case WM_COMMAND:
			if (menu_edit_mode.callback(wparam, lparam)) return 0;
			if (menu_more.callback(wparam, lparam)) return 0;
			switch (wparam) {
			case (WPARAM)WindowCommand::Update:
				// モードボタンの更新
				button_mode.set_label(global::config.get_edit_mode_str());
				if (global::config.get_edit_mode() == EditMode::Script) {
					button_fit.set_status(mkaul::flag::Status::Disabled);
				}
				else {
					button_fit.set_status(mkaul::flag::Status::Null);
				}
				// ID操作盤を表示し、パラメータボタンを非表示にする
				if (
					global::config.get_edit_mode() == EditMode::Normal
					or global::config.get_edit_mode() == EditMode::Value
					or global::config.get_edit_mode() == EditMode::Script
					) {
					button_read.set_status(mkaul::flag::Status::Disabled);
					row_id.show();
					button_param.hide();
					row_bottom.replace(1, &row_id);
					toolbar.move(mkaul::WindowRectangle{ rect_wnd });
				}
				// パラメータボタンを表示し、ID操作盤を非表示にする
				else {
					button_read.set_status(mkaul::flag::Status::Null);
					row_id.hide();
					button_param.show();
					row_bottom.replace(1, &button_param);
					toolbar.move(mkaul::WindowRectangle{ rect_wnd });
				}
				break;

			case (WPARAM)WindowCommand::AddUpdateNotification:

				break;

			case BUTTON_ID_MODE:
			{
				mkaul::WindowRectangle rect_tmp;
				button_mode.get_rect(&rect_tmp);
				POINT point_tmp = { rect_tmp.right / 2, 0 };
				::ClientToScreen(button_mode.get_hwnd(), &point_tmp);
				mkaul::Point<LONG> point_screen{ point_tmp };
				menu_edit_mode.show(hwnd, TPM_CENTERALIGN, &point_screen);
				break;
			}

			case BUTTON_ID_PARAM:
				break;

			case BUTTON_ID_COPY:
			{
				auto tmp = std::to_string(global::editor.track_param());
				if (!util::copy_to_clipboard(hwnd, tmp.c_str()) and global::config.get_show_popup()) {
					my_messagebox(global::string_table[StringId::ErrorCodeCopyFailed], hwnd, MessageBoxIcon::Error);
				}
			}
				break;

			case BUTTON_ID_READ:
			{
				CurveCodeDialog dialog;
				dialog.show(hwnd);
				break;
			}
				
			case BUTTON_ID_SAVE:
				break;

			case BUTTON_ID_CLEAR:
			{
				int response = IDOK;
				if (global::config.get_show_popup()) {
					response = my_messagebox(
						global::string_table[StringId::WarningDeleteCurve],
						hwnd,
						MessageBoxIcon::Warning,
						MessageBoxButton::OkCancel
					);
				}

				if (response == IDOK) {
					// TODO: スクリプトカーブに対応させる
					auto curve = global::editor.editor_graph().current_curve();
					if (curve) {
						curve->clear();
						global::window_main.send_command((WPARAM)WindowCommand::Update);
					}

				}

				break;
			}

			case BUTTON_ID_FIT:
				global::window_grapheditor.send_command((WPARAM)WindowCommand::Fit);
				break;

			case BUTTON_ID_MORE:
				menu_more.show(hwnd);
				break;
			}
			return 0;
		}
		return ::DefWindowProc(hwnd, message, wparam, lparam);
	}
}