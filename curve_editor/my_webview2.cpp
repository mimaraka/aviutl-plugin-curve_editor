#include "my_webview2.hpp"
#include <WebView2EnvironmentOptions.h>
#include <strconv2.h>
#include "config.hpp"
#include "global.hpp"
#include "host_object_editor_graph.hpp"
#include "host_object_editor_script.hpp"
#include "host_object_config.hpp"
#include "host_object_editor.hpp"
#include "my_messagebox.hpp"
#include "webmessage_handler.hpp"
#include "string_table.hpp"



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

								webview_->add_NavigationCompleted(
									Microsoft::WRL::Callback<ICoreWebView2NavigationCompletedEventHandler>(
										[this](ICoreWebView2*, ICoreWebView2NavigationCompletedEventArgs* args) -> HRESULT {
											BOOL success;
											args->get_IsSuccess(&success);
											if (!success) {
												my_messagebox(global::string_table[StringId::ErrorPageLoadFailed], hwnd_, MessageBoxIcon::Error);
											}
											return S_OK;
										}
									).Get(), nullptr
								);

								webview_->add_WebMessageReceived(Microsoft::WRL::Callback<ICoreWebView2WebMessageReceivedEventHandler>(
									[this](ICoreWebView2*, ICoreWebView2WebMessageReceivedEventArgs* args) -> HRESULT {
										LPWSTR json;
										args->get_WebMessageAsJson(&json);
										auto json_obj = nlohmann::json::parse(::wide_to_utf8(json));
										handle_webmessage(global::fp->dll_hinst, hwnd_, webview_.get(), json_obj);
										return S_OK;
									}).Get(), nullptr
								);

								is_ready_ = true;
								update_color_scheme();
								add_host_object<GraphEditorHostObject>(L"graphEditor");
								add_host_object<ScriptEditorHostObject>(L"scriptEditor");
								add_host_object<ConfigHostObject>(L"config");
								add_host_object<EditorHostObject>(L"editor");
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

	void MyWebView2::navigate(const std::wstring& html_name) {
		if (is_ready_) {
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

	void MyWebView2::update_color_scheme() noexcept {
		wil::com_ptr<ICoreWebView2_22> webview2_22 = webview_.try_query<ICoreWebView2_22>();
		if (webview2_22) {
			wil::com_ptr<ICoreWebView2Profile> profile;
			webview2_22->get_Profile(&profile);

			COREWEBVIEW2_PREFERRED_COLOR_SCHEME color_scheme;
			switch (global::config.get_theme_id()) {
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
} // namespace cved