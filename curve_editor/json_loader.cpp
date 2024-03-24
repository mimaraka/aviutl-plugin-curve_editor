#include "json_loader.hpp"
#include <fstream>



namespace cved {
	void JsonLoader::get_json_path(HINSTANCE hinst, std::filesystem::path* p_path) noexcept {
		char dll_path_str[MAX_PATH];
		::GetModuleFileNameA(hinst, dll_path_str, MAX_PATH);
		*p_path = std::filesystem::path{ dll_path_str }.parent_path() / JSON_NAME;
	}

	bool JsonLoader::get_data(HINSTANCE hinst, const char* key, nlohmann::json* p_data) noexcept {
		using json = nlohmann::json;
		std::filesystem::path json_path;
		get_json_path(hinst, &json_path);
		std::ifstream file{ json_path };

		if (file.fail()) return false;
		try {
			json root = json::parse(file);
			if (root.contains(key)) {
				*p_data = root[key];
				return true;
			}
			else return false;
		}
		catch (json::parse_error&) {
			return false;
		}
	}

	bool JsonLoader::set_data(HINSTANCE hinst, const char* key, const nlohmann::json& data) noexcept {
		using json = nlohmann::json;
		std::filesystem::path json_path;
		get_json_path(hinst, &json_path);
		std::ifstream file_in{ json_path };
		json root = json::object();

		if (!file_in.fail()) {
			try {
				root = json::parse(file_in);
			}
			catch (json::parse_error&) {}
		}
		root[key] = data;
		file_in.close();
		std::ofstream file_out{ json_path };
		if (file_out.fail()) return false;
		file_out << root.dump(4);
		return true;
	}
}