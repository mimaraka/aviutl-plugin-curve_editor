#pragma once

#include <filesystem>
#include <Windows.h>
#include <nlohmann/json.hpp>



namespace cved {
	class JsonLoader {
		static constexpr char JSON_NAME[] = "curve_editor.json";

	public:
		static void get_json_path(HINSTANCE hinst, std::filesystem::path* p_path) noexcept;
		template<typename T>
		static bool get_value(const nlohmann::json& data, const char* key, T& var) noexcept;

		static bool get_data(HINSTANCE hinst, const char* key, nlohmann::json* p_data) noexcept;
		static bool set_data(HINSTANCE hinst, const char* key, const nlohmann::json& data) noexcept;
	};

	template<typename T>
	static bool JsonLoader::get_value(const nlohmann::json& data, const char* key, T& var) noexcept {
		if (data.contains(key)) {
			try {
				var = data[key].get<T>();
				return true;
			}
			catch (nlohmann::json::type_error&) {}
		}
		return false;
	}
}