#include "config.hpp"
#include "global.hpp"
#include "host_object_config.hpp"
#include "host_object_editor.hpp"
#include "host_object_preset.hpp"
#include "message_box.hpp"
#include "message_handler.hpp"
#include "my_webview2.hpp"
#include "string_table.hpp"
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
					env->QueryInterface(IID_PPV_ARGS(&env_));

					auto hr = env->CreateCoreWebView2Controller(
						hwnd_,
						Microsoft::WRL::Callback<ICoreWebView2CreateCoreWebView2ControllerCompletedHandler>(
							[this, after_callback](HRESULT result, ICoreWebView2Controller* controller) -> HRESULT {
								if (FAILED(result)) {
									return result;
								}
								controller->QueryInterface(IID_PPV_ARGS(&controller_));
								controller->get_CoreWebView2(&webview_);
								controller->put_IsVisible(TRUE);
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
										MessageHandler{ hwnd_, this }.handle_message(json_obj);
										return S_OK;
									}).Get(), nullptr
								);

								is_ready_ = true;
								update_color_scheme();
								add_host_object<ConfigHostObject>(L"config");
								add_host_object<EditorHostObject>(L"editor");
								add_host_object<PresetHostObject>(L"preset");
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

	void MyWebView2::navigate(std::function<void(MyWebView2*)> after_callback) {
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
							util::message_box(global::string_table[StringId::ErrorPageLoadFailed], hwnd_, util::MessageBoxIcon::Error);
						}
						return S_OK;
					}
				).Get(), nullptr
			);
			webview_->Navigate(std::format(L"{}\\ui\\index.html", global::config.get_dir_plugin().wstring()).c_str());
		}
	}

	void MyWebView2::reload() noexcept {
		if (is_ready_) {
			webview_->Reload();
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

	void MyWebView2::post_message(const std::wstring& command, const nlohmann::json& options) const noexcept {
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
			std::format(L"{{\"command\":\"{}\"{}}}", command, options_str).c_str()
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
} // namespace cved