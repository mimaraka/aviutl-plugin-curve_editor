#pragma once

#include "string_table.hpp"
#include <functional>
#include <nlohmann/json.hpp>
#include <string>
#include <vector>



namespace curve_editor {
	// 環境設定の各項目に対応するウィジェットの種類
	enum class PrefWidget {
		Checkbox,
		Slider,
		Combo,
		Color,
		FilePath,
		Text
	};

	// 環境設定項目1つ分の記述子
	// getter/setter は nlohmann::json 境界で型消去し、global::config を読み書きする
	struct PrefDescriptor {
		std::string key;									// Preferences::to_json と一致させるキー
		PrefWidget widget;									// ウィジェットの種類
		global::StringTable::StringId label;				// ローカライズ用ラベル
		global::StringTable::StringId category;				// 所属カテゴリ
		double min = 0.0, max = 0.0, step = 0.0;			// 数値系ウィジェット用
		std::vector<std::wstring> options;					// combo の選択肢(解決済み表示文字列、index=値)
		std::function<nlohmann::json()> getter;				// global::config を読む
		std::function<void(const nlohmann::json&)> setter;	// global::config に書く
	};

	// 18項目の記述子テーブルを返す(初回呼び出し時に string_table を用いて構築)
	const std::vector<PrefDescriptor>& pref_schema();

	// label/category を string_table で解決した JSON スキーマ(配列)を構築
	nlohmann::json build_schema_json();

	// 全項目の現在値 { key: getter() } を JSON で構築
	nlohmann::json build_values_json();

	// JSON をパースして該当 setter を呼ぶ(存在するキーのみ)
	void apply_values_json(const nlohmann::json& values);

	// デフォルト値のみを計算した JSON を構築(global::config は変更しない)
	nlohmann::json build_default_values_json();
} // namespace curve_editor
