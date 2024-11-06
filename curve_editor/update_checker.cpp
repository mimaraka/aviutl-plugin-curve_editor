#include "update_checker.hpp"
#include <mkaul/http.hpp>
#include <nlohmann/json.hpp>
#include <strconv2.h>



namespace cved::global {
	bool UpdateChecker::fetch_latest_version() {
		mkaul::http::Client client;
		client.open("https://api.github.com/repos/mimaraka/aviutl-plugin-curve_editor/releases/latest");
		std::vector<byte> stream;
		DWORD status_code;
		client.get(&stream, &status_code);
		std::string json_str(stream.begin(), stream.end());
		try {
			auto json = nlohmann::json::parse(json_str);
			std::string tag_name = json.at("tag_name");
			release_notes_ = ::utf8_to_sjis(json.at("body").get<std::string>());
			return latest_version_.from_str(tag_name);
		}
		catch (const std::exception&) {
			return false;
		}
	}

	void UpdateChecker::check_for_update() noexcept {
		th_ = std::thread{ [this] {
			if (fetch_latest_version()) {
				if (PLUGIN_VERSION < latest_version_) {
					is_update_available_ = true;
					if (webview) webview->send_command(MessageCommand::NotifyUpdateAvailable);
				}
			}
		} };
	}
} // namespace cved::global