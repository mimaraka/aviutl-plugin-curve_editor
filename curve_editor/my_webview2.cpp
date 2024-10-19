#include "config.hpp"
#include "dialog_curve_code.hpp"
#include "dialog_curve_param.hpp"
#include "dialog_id_jumpto.hpp"
#include "global.hpp"
#include "host_object_config.hpp"
#include "host_object_editor_graph.hpp"
#include "host_object_editor_script.hpp"
#include "menu_curve_segment.hpp"
#include "menu_graph.hpp"
#include "menu_others.hpp"
#include "my_messagebox.hpp"
#include "my_webview2.hpp"
#include "string_table.hpp"
#include "util.hpp"
#include <strconv2.h>
#include <WebView2EnvironmentOptions.h>



namespace cved {
	bool MyWebView2::init(HWND hwnd, std::function<void(MyWebView2*)> after_callback) {
		using StringId = global::StringTable::StringId;

		hwnd_ = hwnd;

		if (is_ready_) {
			return false;
		}

		auto options = Microsoft::WRL::Make<CoreWebView2EnvironmentOptions>();
		options->put_AreBrowserExtensionsEnabled(FALSE);
		options->put_ExclusiveUserDataFolderAccess(FALSE);
		options->put_AdditionalBrowserArguments(L"--disable-web-security");
		options->put_EnableTrackingPrevention(TRUE);

		auto hr = ::CreateCoreWebView2EnvironmentWithOptions(
			nullptr,
			global::config.get_dir_plugin().wstring().c_str(),
			options.Get(),
			Microsoft::WRL::Callback<ICoreWebView2CreateCoreWebView2EnvironmentCompletedHandler>(
				[this, after_callback](HRESULT result, ICoreWebView2Environment* env) -> HRESULT {
					if (FAILED(result)) {
						return result;
					}
					env_ = env;

					auto hr = env->CreateCoreWebView2Controller(
						hwnd_,
						Microsoft::WRL::Callback<ICoreWebView2CreateCoreWebView2ControllerCompletedHandler>(
							[this, after_callback](HRESULT result, ICoreWebView2Controller* controller) -> HRESULT {
								if (FAILED(result)) {
									return result;
								}
								controller_ = controller;
								controller->get_CoreWebView2(&webview_);
								wil::com_ptr<ICoreWebView2Settings> settings;
								webview_->get_Settings(&settings);
								settings->put_AreHostObjectsAllowed(TRUE);
								settings->put_AreDefaultContextMenusEnabled(FALSE);
								settings->put_IsScriptEnabled(TRUE);
								settings->put_IsZoomControlEnabled(FALSE);
								settings->put_IsBuiltInErrorPageEnabled(FALSE);
								settings->put_IsStatusBarEnabled(FALSE);
#ifdef _DEBUG
								settings->put_AreDevToolsEnabled(TRUE);
#else
								settings->put_AreDevToolsEnabled(FALSE);
								auto settings3 = settings.try_query<ICoreWebView2Settings3>();
								if (settings3) {
									settings3->put_AreBrowserAcceleratorKeysEnabled(FALSE);
								}
#endif
								webview_->AddScriptToExecuteOnDocumentCreated(L"\
									window.addEventListener('dragover', (e) => {e.preventDefault();}, false);\
									window.addEventListener('drop', (e) => {e.preventDefault();}, false);",
									nullptr
								);

								webview_->add_WebMessageReceived(Microsoft::WRL::Callback<ICoreWebView2WebMessageReceivedEventHandler>(
									[this](ICoreWebView2*, ICoreWebView2WebMessageReceivedEventArgs* args) -> HRESULT {
										LPWSTR json;
										args->get_WebMessageAsJson(&json);
										auto json_obj = nlohmann::json::parse(::wide_to_utf8(json));
										handle_message(json_obj);
										return S_OK;
									}).Get(), nullptr
								);

								is_ready_ = true;
								update_color_scheme();
								add_host_object<GraphEditorHostObject>(L"graphEditor");
								add_host_object<ScriptEditorHostObject>(L"scriptEditor");
								add_host_object<ConfigHostObject>(L"config");
								after_callback(this);
								return S_OK;
							}
						).Get()
					);
					return hr;
				}
			).Get()
		);
		if (FAILED(hr)) {
			return false;
		}
		return true;
	}

	void MyWebView2::destroy() noexcept {
		if (is_ready_) {
			controller_->Close();
			is_ready_ = false;
			hwnd_ = NULL;
		}
	}

	void MyWebView2::navigate(const std::wstring& html_name, std::function<void(MyWebView2*)> after_callback) {
		using StringId = global::StringTable::StringId;
		if (is_ready_) {
			webview_->add_NavigationCompleted(
				Microsoft::WRL::Callback<ICoreWebView2NavigationCompletedEventHandler>(
					[this, after_callback](ICoreWebView2*, ICoreWebView2NavigationCompletedEventArgs* args) -> HRESULT {
						BOOL success;
						args->get_IsSuccess(&success);
						if (success) {
							if (after_callback) {
								after_callback(this);
							}
						} else {
							my_messagebox(global::string_table[StringId::ErrorPageLoadFailed], hwnd_, MessageBoxIcon::Error);
						}
						return S_OK;
					}
				).Get(), nullptr
			);

			std::filesystem::path ce_dir;
#ifdef _DEBUG
			std::filesystem::path this_path = __FILE__;
			ce_dir = this_path.parent_path();
#else
			ce_dir = global::config.get_dir_plugin();
#endif
			webview_->Navigate(std::format(L"{}\\ui\\{}.html", ce_dir.wstring(), html_name).c_str());
		}
	}

	void MyWebView2::reload() noexcept {
		if (is_ready_) {
			webview_->Reload();
		}
	}

	void MyWebView2::execute_script(const std::wstring& script) {
		if (is_ready_) {
			webview_->ExecuteScript(script.c_str(), nullptr);
		}	
	}

	void MyWebView2::put_bounds(const mkaul::WindowRectangle& bounds) {
		if (is_ready_) {
			controller_->put_Bounds(bounds.get_rect());
		}
	}

	void MyWebView2::on_move() noexcept {
		if (is_ready_) {
			controller_->NotifyParentWindowPositionChanged();
		}
	}

	void MyWebView2::post_message(const std::wstring& to, const std::wstring& command, const nlohmann::json& options) const noexcept {
		if (!is_ready_) {
			return;
		}
		std::wstring options_str;
		if (!options.empty()) {
			options_str = ::utf8_to_wide(options.dump());
			options_str.pop_back();
			options_str.replace(0, 1, L",");
		}
		webview_->PostWebMessageAsJson(
			std::format(L"{{\"to\":\"{}\",\"command\":\"{}\"{}}}", to, command, options_str).c_str()
		);
	}

	void MyWebView2::update_color_scheme() noexcept {
		wil::com_ptr<ICoreWebView2_22> webview2_22 = webview_.try_query<ICoreWebView2_22>();
		if (webview2_22) {
			wil::com_ptr<ICoreWebView2Profile> profile;
			webview2_22->get_Profile(&profile);

			COREWEBVIEW2_PREFERRED_COLOR_SCHEME color_scheme;
			switch (global::config.get_theme()) {
			case ThemeId::Light:
				color_scheme = COREWEBVIEW2_PREFERRED_COLOR_SCHEME_LIGHT;
				break;

			case ThemeId::Dark:
				color_scheme = COREWEBVIEW2_PREFERRED_COLOR_SCHEME_DARK;
				break;

			default:
				color_scheme = COREWEBVIEW2_PREFERRED_COLOR_SCHEME_AUTO;
			}
			profile->put_PreferredColorScheme(color_scheme);
		}
	}

	bool MyWebView2::handle_message(const nlohmann::json& message) noexcept {
		using StringId = global::StringTable::StringId;

		try {
			auto command = message.at("command").get<std::string>();
			if (command == "copy") {
				auto tmp = std::to_string(global::editor.track_param());
				if (!util::copy_to_clipboard(hwnd_, tmp.c_str()) and global::config.get_show_popup()) {
					my_messagebox(global::string_table[StringId::ErrorCodeCopyFailed], hwnd_, MessageBoxIcon::Error);
				}
				return true;
			}
			else if (command == "read") {
				CurveCodeDialog dialog;
				dialog.show(hwnd_);
				return true;
			}
			else if (command == "save") {
				return true;
			}
			else if (command == "lock") {
				bool sw = message.at("switch").get<bool>();
				if (sw) {
					return true;
				}
				else {
					return true;
				}
			}
			else if (command == "clear") {
				int response = IDOK;
				if (global::config.get_show_popup()) {
					response = my_messagebox(
						global::string_table[StringId::WarningDeleteCurve],
						hwnd_,
						MessageBoxIcon::Warning,
						MessageBoxButton::OkCancel
					);
				}

				if (response == IDOK) {
					auto curve = global::editor.current_curve();
					if (curve) {
						curve->clear();
						post_message(L"editor-graph", L"updateHandles");
					}
				}
				return true;
			}
			else if (command == "others") {
				OthersMenu menu(global::fp->dll_hinst);
				menu.show(hwnd_);
				return true;
			}
			else if (command == "button-id") {
				IdJumptoDialog dialog;
				dialog.show(hwnd_);
				return true;
			}
			else if (command == "button-param") {
				CurveParamDialog dialog;
				dialog.show(hwnd_);
				return true;
			}
			else if (command == "drag-and-drop") {
				::SendMessageA(hwnd_, WM_COMMAND, (WPARAM)WindowCommand::StartDnd, NULL);
				return true;
			}
			else if (command == "drag-end") {
				auto curve_id = message.at("curveId").get<uint32_t>();
				auto curve_numeric = global::id_manager.get_curve<NumericGraphCurve>(curve_id);
				if (curve_numeric) {
					if (global::config.get_auto_copy()) {
						auto code = std::to_string(curve_numeric->encode());
						if (!util::copy_to_clipboard(hwnd_, code.c_str()) and global::config.get_show_popup()) {
							my_messagebox(global::string_table[StringId::ErrorCodeCopyFailed], hwnd_, MessageBoxIcon::Error);
						}
					}
				}
				else {
					if (global::config.get_auto_apply()) {
						::SendMessageA(global::fp->hwnd, WM_COMMAND, (WPARAM)WindowCommand::RedrawAviutl, 0);
					}
				}
			}
			else if (command == "contextmenu-graph") {
				auto mode = message.at("mode").get<EditMode>();
				auto curve_id = message.at("curveId").get<uint32_t>();
				GraphMenu menu{ global::fp->dll_hinst };
				menu.show(mode, curve_id, *this, hwnd_);
			}
			else if (command == "contextmenu-curve-segment") {
				auto id = message.at("curveId").get<uint32_t>();
				auto segment_id = message.at("segmentId").get<uint32_t>();
				CurveSegmentMenu menu{ global::fp->dll_hinst };
				menu.show(id, segment_id, *this, hwnd_);
			}
			else if (command == "selectdlg-ok") {
				auto mode = message.at("mode").get<EditMode>();
				auto id_or_idx = message.at("idOrIdx").get<uint32_t>();
				int param = 0;
				std::pair<std::string, int> ret;

				switch (mode) {
				case EditMode::Normal:
				case EditMode::Value:
				case EditMode::Script:
					param = (int)id_or_idx + 1;
					break;

				case EditMode::Bezier:
				case EditMode::Elastic:
				case EditMode::Bounce:
				{
					auto curve = global::id_manager.get_curve<NumericGraphCurve>(id_or_idx);
					if (curve) {
						param = curve->encode();
					}
					break;
				}
				}
				ret = std::make_pair(global::editor.get_curve(mode)->get_type(), param);
				::SendMessageA(hwnd_, WM_COMMAND, (WPARAM)WindowCommand::SetCurveInfo, std::bit_cast<LPARAM>(&ret));
				::DestroyWindow(hwnd_);
			}
			else if (command == "selectdlg-cancel") {
				::DestroyWindow(hwnd_);
			}
		}
		catch (const nlohmann::json::exception&) {}
		return false;
	}
} // namespace cved