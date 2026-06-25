#pragma once

#include <functional>
#include <nlohmann/json.hpp>
#include <Windows.h>



namespace curve_editor {
	enum class MessageCommand {
		LoadStringTable,
		InitComponent,
		ButtonCopy,
		ButtonCurveCode,
		ButtonSave,
		ButtonClear,
		ButtonOthers,
		ButtonIdx,
		ButtonParam,
		ButtonPresetListSetting,
		ContextMenuGraph,
		ContextMenuSegment,
		ContextMenuBezierHandle,
		ContextMenuElasticHandle,
		ContextMenuBounceHandle,
		ContextMenuPreset,
		ContextMenuPresetItem,
		ContextMenuIdx,
		JumpToIdx,
		ButtonCollectionAdd,
		ButtonCollection,
		SelectCurveOk,
		SelectCurveCancel,
		OnDndStart,
		OnDndEnd,
		OnCurveEdit,
		ApplyPreset,
		UpdateControl,
		UpdateHandlePosition,
		UpdateCurvePath,
		UpdateEditor,
		UpdateAxisLabelVisibility,
		UpdateHandleVisibility,
		UpdateVelocityGraphVisibility,
		UpdatePresetLayout,
		ApplyPreferences,
		UpdatePresets,
		ChangeEditMode,
		ChangeLayoutMode,
		NotifyUpdateAvailable,
		PreferencesApply,
		PreferencesOk,
		PreferencesCancel,
		PickColor,
		PickFilePath,
		SetPrefValue,
	};

	using MessageHandlerCallback = std::function<void(const nlohmann::json&)>;
	
	class MyWebView2;
	class MessageHandler {
		std::vector<std::pair<MessageCommand, MessageHandlerCallback>> handlers_;
		HWND hwnd_;
		MyWebView2* p_webview_;

		void button_copy();
		void button_curve_code();
		void button_save();
		void button_clear();
		void button_others();
		void button_idx();
		void button_param(const nlohmann::json& options);
		void context_menu_graph(const nlohmann::json& options);
		void context_menu_segment(const nlohmann::json& options);
		void context_menu_bezier_handle(const nlohmann::json& options);
		void context_menu_elastic_handle(const nlohmann::json& options);
		void context_menu_bounce_handle(const nlohmann::json& options);
		void context_menu_preset();
		void context_menu_preset_item(const nlohmann::json& options);
		void context_menu_idx();
		void jump_to_idx(const nlohmann::json& options);
		void button_collection_add();
		void button_collection();
		void button_preset_list_setting();
		void select_curve_ok(const nlohmann::json& options);
		void select_curve_cancel();
		void on_dnd_start(const nlohmann::json& options);
		void on_curve_edit(const nlohmann::json& options);
		void apply_preset(const nlohmann::json& options);
		void apply_preferences_internal(const nlohmann::json& options);
		void preferences_apply(const nlohmann::json& options);
		void preferences_ok(const nlohmann::json& options);
		void preferences_cancel();
		void pick_color(const nlohmann::json& options);
		void pick_file_path(const nlohmann::json& options);

	public:
		MessageHandler(HWND hwnd, MyWebView2* p_webview) : hwnd_{ hwnd }, p_webview_{p_webview} {
			handlers_.emplace_back(MessageCommand::ButtonCopy, std::bind(&MessageHandler::button_copy, this));
			handlers_.emplace_back(MessageCommand::ButtonCurveCode, std::bind(&MessageHandler::button_curve_code, this));
			handlers_.emplace_back(MessageCommand::ButtonSave, std::bind(&MessageHandler::button_save, this));
			handlers_.emplace_back(MessageCommand::ButtonClear, std::bind(&MessageHandler::button_clear, this));
			handlers_.emplace_back(MessageCommand::ButtonOthers, std::bind(&MessageHandler::button_others, this));
			handlers_.emplace_back(MessageCommand::ButtonIdx, std::bind(&MessageHandler::button_idx, this));
			handlers_.emplace_back(MessageCommand::ButtonParam, std::bind(&MessageHandler::button_param, this, std::placeholders::_1));
			handlers_.emplace_back(MessageCommand::ContextMenuGraph, std::bind(&MessageHandler::context_menu_graph, this, std::placeholders::_1));
			handlers_.emplace_back(MessageCommand::ContextMenuSegment, std::bind(&MessageHandler::context_menu_segment, this, std::placeholders::_1));
			handlers_.emplace_back(MessageCommand::ContextMenuBezierHandle, std::bind(&MessageHandler::context_menu_bezier_handle, this, std::placeholders::_1));
			handlers_.emplace_back(MessageCommand::ContextMenuElasticHandle, std::bind(&MessageHandler::context_menu_elastic_handle, this, std::placeholders::_1));
			handlers_.emplace_back(MessageCommand::ContextMenuBounceHandle, std::bind(&MessageHandler::context_menu_bounce_handle, this, std::placeholders::_1));
			handlers_.emplace_back(MessageCommand::ContextMenuPreset, std::bind(&MessageHandler::context_menu_preset, this));
			handlers_.emplace_back(MessageCommand::ContextMenuPresetItem, std::bind(&MessageHandler::context_menu_preset_item, this, std::placeholders::_1));
			handlers_.emplace_back(MessageCommand::ContextMenuIdx, std::bind(&MessageHandler::context_menu_idx, this));
			handlers_.emplace_back(MessageCommand::JumpToIdx, std::bind(&MessageHandler::jump_to_idx, this, std::placeholders::_1));
			handlers_.emplace_back(MessageCommand::ButtonCollectionAdd, std::bind(&MessageHandler::button_collection_add, this));
			handlers_.emplace_back(MessageCommand::ButtonCollection, std::bind(&MessageHandler::button_collection, this));
			handlers_.emplace_back(MessageCommand::ButtonPresetListSetting, std::bind(&MessageHandler::button_preset_list_setting, this));
			handlers_.emplace_back(MessageCommand::SelectCurveOk, std::bind(&MessageHandler::select_curve_ok, this, std::placeholders::_1));
			handlers_.emplace_back(MessageCommand::SelectCurveCancel, std::bind(&MessageHandler::select_curve_cancel, this));
			handlers_.emplace_back(MessageCommand::OnDndStart, std::bind(&MessageHandler::on_dnd_start, this, std::placeholders::_1));
			handlers_.emplace_back(MessageCommand::OnCurveEdit, std::bind(&MessageHandler::on_curve_edit, this, std::placeholders::_1));
			handlers_.emplace_back(MessageCommand::ApplyPreset, std::bind(&MessageHandler::apply_preset, this, std::placeholders::_1));
			handlers_.emplace_back(MessageCommand::PreferencesApply, std::bind(&MessageHandler::preferences_apply, this, std::placeholders::_1));
			handlers_.emplace_back(MessageCommand::PreferencesOk, std::bind(&MessageHandler::preferences_ok, this, std::placeholders::_1));
			handlers_.emplace_back(MessageCommand::PreferencesCancel, std::bind(&MessageHandler::preferences_cancel, this));
			handlers_.emplace_back(MessageCommand::PickColor, std::bind(&MessageHandler::pick_color, this, std::placeholders::_1));
			handlers_.emplace_back(MessageCommand::PickFilePath, std::bind(&MessageHandler::pick_file_path, this, std::placeholders::_1));
		}

		bool handle_message(const nlohmann::json& message);
		void send_command(MessageCommand command, const nlohmann::json& options = nlohmann::json::object());
	};
} // namespace curve_editor