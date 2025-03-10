#include "actctx_helper.hpp"
#include "config.hpp"
#include "constants.hpp"
#include "curve_editor.hpp"
#include "drag_and_drop.hpp"
#include "exedit_hook.hpp"
#include "filter_wndproc.hpp"
#include "message_box.hpp"
#include "my_webview2.hpp"
#include "my_webview2_reference.hpp"
#include "resource.h"
#include "string_table.hpp"
#include "update_checker.hpp"
#include <nlohmann/json.hpp>



namespace curve_editor {
	// ウィンドウプロシージャ
	BOOL filter_wndproc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam, AviUtl::EditHandle*, AviUtl::FilterPlugin* fp) {
		using WindowMessage = AviUtl::FilterPlugin::WindowMessage;
		using StringId = global::StringTable::StringId;

		static MyWebView2 my_webview;
		static ActCtxHelper actctx_helper;
		static DragAndDrop dnd;
		static bool dnd_init_done = false;

		switch (message) {
			// ウィンドウ作成時
		case WindowMessage::Init:
			actctx_helper.init();
			my_webview.init(hwnd, [](MyWebView2* this_) {
				mkaul::WindowRectangle bounds;
				bounds.from_client_rect(this_->get_hwnd());
				this_->put_bounds(bounds);
				this_->navigate([](MyWebView2* this_) {
					this_->send_command(MessageCommand::InitComponent, {
						{"page", "MainPanel"},
						{"isUpdateAvailable", global::update_checker.is_update_available()}
						});
					});
				});
			global::webview.set(global::MyWebView2Reference::WebViewType::Main, my_webview);
			// Alpha版・Beta版の場合はキャプションを変更
			if (global::PLUGIN_VERSION.is_preview()) {
				::SetWindowTextW(
					hwnd,
					std::format(L"{} ({})", global::PLUGIN_DISPLAY_NAME, ::sjis_to_wide(global::PLUGIN_VERSION.preview_type().str(true))).c_str()
				);
			}
			break;

			// AviUtl終了時
		case WindowMessage::Exit:
			actctx_helper.exit();
			dnd.exit();
			break;

			// 拡張編集フックに関する初期化処理
			// curve_editor.aufの配置場所によっては、WindowMessage::Init時点ではexedit.aufが読み込まれていない可能性がある
		case WindowMessage::ChangeWindow:
			if (!dnd_init_done) {
				dnd.init();
				dnd_init_done = true;
			}
			break;

		case WindowMessage::FileClose:
			global::editor.reset_id_curves();
			my_webview.send_command(MessageCommand::UpdateEditor);
			break;

		case WM_SIZE:
		{
			mkaul::WindowRectangle bounds;
			bounds.from_client_rect(hwnd);
			my_webview.put_bounds(bounds);
			break;
		}

		case WM_MOVE:
			my_webview.on_move();
			break;

		case WM_MOUSEMOVE:
			if (dnd.is_dragging()) {
				dnd.update();
			}
			break;

		case WM_LBUTTONUP:
			if (dnd.is_dragging()) {
				::ReleaseCapture();
				dnd.drop();
				my_webview.send_command(MessageCommand::OnDndEnd);
			}
			break;

		case WM_COMMAND:
			switch ((WindowCommand)wparam) {
			case WindowCommand::RedrawAviutl:
				return TRUE;

			case WindowCommand::StartDnd:
				::SetCapture(hwnd);
				::SetCursor(::LoadCursorA(fp->dll_hinst, MAKEINTRESOURCEA(IDC_DRAG)));
				dnd.drag(static_cast<uint32_t>(lparam));
				break;
			}
			break;
		}
		return FALSE;
	}
} // namespace curve_editor