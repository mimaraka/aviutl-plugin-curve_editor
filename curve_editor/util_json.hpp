#pragma once

#include <nlohmann/json.hpp>



namespace cved {
	template<typename C, typename T, typename Ret>
	bool set_from_json(C* this_, const nlohmann::json& json, const std::string& key, Ret(C::* setter)(T)) noexcept {
		try {
			(this_->*setter)(json.at(key).get<T>());
			return true;
		}
		catch (const nlohmann::json::exception&) {}
		return false;
	}

	template<typename T>
	bool set_from_json(const nlohmann::json& json, const std::string& key, T& variable) noexcept {
		try {
			variable = json.at(key).get<T>();
			return true;
		}
		catch (const nlohmann::json::exception&) {}
		return false;
	}
}