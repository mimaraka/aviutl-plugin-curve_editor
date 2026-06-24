#pragma once

#include "preferences_schema.hpp"
#include <mkaul/host_object.hpp>
#include <nlohmann/json.hpp>
#include <strconv2.h>



namespace curve_editor {
	class PreferencesHostObject : public mkaul::ole::HostObject {
		// スキーマ(label/category 解決済み)を JSON 文字列で返す
		static std::wstring get_schema_json() {
			return ::utf8_to_wide(build_schema_json().dump());
		}

		// 全項目の現在値を JSON 文字列で返す
		static std::wstring get_values_json() {
			return ::utf8_to_wide(build_values_json().dump());
		}

		// JSON 文字列をパースして global::config に一括適用する
		static void set_values_json(std::wstring json) {
			try {
				apply_values_json(nlohmann::json::parse(::wide_to_utf8(json)));
			}
			catch (const nlohmann::json::exception&) {}
		}

		// デフォルト値のみを JSON 文字列で返す(global::config は変更しない)
		static std::wstring get_default_values_json() {
			return ::utf8_to_wide(build_default_values_json().dump());
		}

	public:
		PreferencesHostObject() {
			register_member(L"getSchemaJson", DispatchType::Method, get_schema_json);
			register_member(L"getValuesJson", DispatchType::Method, get_values_json);
			register_member(L"setValuesJson", DispatchType::Method, set_values_json);
			register_member(L"getDefaultValuesJson", DispatchType::Method, get_default_values_json);
		}
	};
} // namespace curve_editor
