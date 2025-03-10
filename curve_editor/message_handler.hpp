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
		ContextMenuPresetItem,
		ContextMenuIdx,
		ButtonCollectionAdd,
		ButtonCollection,
		SelectCurveOk,
		SelectCurveCancel,
		OnDndStart,
		OnDndEnd,
		OnHandleDragEnd,
		ApplyPreset,
		UpdateControl,
		UpdateHandlePosition,
		UpdateCurvePath,
		UpdateEditor,
		UpdateAxisLabelVisibility,
		UpdateHandleVisibility,
		UpdateVelocityGraphVisibility,
		ApplyPreferences,
		UpdatePresets,
		ChangeEditMode,
		ChangeLayoutMode,
		NotifyUpdateAvailable,
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
		void context_menu_preset_item(const nlohmann::json& options);
		void context_menu_idx();
		void button_collection_add();
		void button_collection();
		void button_preset_list_setting();
		void select_curve_ok(const nlohmann::json& options);
		void select_curve_cancel();
		void on_dnd_start(const nlohmann::json& options);
		void on_handle_drag_end(const nlohmann::json& options);
		void apply_preset(const nlohmann::json& options);

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
			handlers_.emplace_back(MessageCommand::ContextMenuPresetItem, std::bind(&MessageHandler::context_menu_preset_item, this, std::placeholders::_1));
			handlers_.emplace_back(MessageCommand::ContextMenuIdx, std::bind(&MessageHandler::context_menu_idx, this));
			handlers_.emplace_back(MessageCommand::ButtonCollectionAdd, std::bind(&MessageHandler::button_collection_add, this));
			handlers_.emplace_back(MessageCommand::ButtonCollection, std::bind(&MessageHandler::button_collection, this));
			handlers_.emplace_back(MessageCommand::ButtonPresetListSetting, std::bind(&MessageHandler::button_preset_list_setting, this));
			handlers_.emplace_back(MessageCommand::SelectCurveOk, std::bind(&MessageHandler::select_curve_ok, this, std::placeholders::_1));
			handlers_.emplace_back(MessageCommand::SelectCurveCancel, std::bind(&MessageHandler::select_curve_cancel, this));
			handlers_.emplace_back(MessageCommand::OnDndStart, std::bind(&MessageHandler::on_dnd_start, this, std::placeholders::_1));
			handlers_.emplace_back(MessageCommand::OnHandleDragEnd, std::bind(&MessageHandler::on_handle_drag_end, this, std::placeholders::_1));
			handlers_.emplace_back(MessageCommand::ApplyPreset, std::bind(&MessageHandler::apply_preset, this, std::placeholders::_1));
		}

		bool handle_message(const nlohmann::json& message);
		void send_command(MessageCommand command, const nlohmann::json& options = nlohmann::json::object());
	};
} // namespace curve_editor