#pragma once

#include "config.hpp"
#include <mkaul/host_object.hpp>



namespace curve_editor {
	class ConfigHostObject : public mkaul::ole::HostObject {
		static std::wstring get_curve_color();

	public:
		ConfigHostObject() {
			register_member(L"editMode", DispatchType::PropertyGet, +[] { return global::config.get_edit_mode(); });
			register_member(L"editMode", DispatchType::PropertyPut, +[](int edit_mode) { global::config.set_edit_mode((EditMode)edit_mode); });
			register_member(L"applyMode", DispatchType::PropertyGet, +[] { return global::config.get_apply_mode(); });
			register_member(L"applyMode", DispatchType::PropertyPut, +[](int apply_mode) { global::config.set_apply_mode((ApplyMode)apply_mode); });
			register_member(L"layoutMode", DispatchType::PropertyGet, +[] { return global::config.get_layout_mode(); });
			register_member(L"editModeNum", DispatchType::PropertyGet, +[] { return (uint32_t)EditMode::NumEditMode; });
			register_member(L"applyModeNum", DispatchType::PropertyGet, +[] { return (uint32_t)ApplyMode::NumApplyMode; });
			register_member(L"getEditModeName", DispatchType::Method, +[](int edit_mode) { return global::config.get_edit_mode_dispname((EditMode)edit_mode); });
			register_member(L"getApplyModeName", DispatchType::Method, +[](int apply_mode) { return global::config.get_apply_mode_dispname((ApplyMode)apply_mode); });
			register_member(L"showXLabel", DispatchType::PropertyGet, +[] {return global::config.get_show_x_label(); });
			register_member(L"showYLabel", DispatchType::PropertyGet, +[] { return global::config.get_show_y_label(); });
			register_member(L"showHandle", DispatchType::PropertyGet, +[] { return global::config.get_show_handle(); });
			register_member(L"showHandle", DispatchType::PropertyPut, +[](bool show_handle) { return global::config.set_show_handle(show_handle); });
			register_member(L"alignHandle", DispatchType::PropertyGet, +[] { return global::config.get_align_handle(); });
			register_member(L"alignHandle", DispatchType::PropertyPut, +[](bool align_handle) { return global::config.set_align_handle(align_handle); });
			register_member(L"enableHotkeys", DispatchType::PropertyGet, +[] { return global::config.get_enable_hotkeys(); });
			register_member(L"showVelocityGraph", DispatchType::PropertyGet, +[] { return global::config.get_show_velocity_graph(); });
			register_member(L"enableAnimation", DispatchType::PropertyGet, +[] { return global::config.get_enable_animation(); });
			register_member(L"wordWrap", DispatchType::PropertyGet, +[] { return global::config.get_word_wrap(); });
			register_member(L"invertWheel", DispatchType::PropertyGet, +[] { return global::config.get_invert_wheel(); });
			register_member(L"setBackgroundImage", DispatchType::PropertyGet, +[] { return global::config.get_show_bg_image(); });
			register_member(L"backgroundImagePath", DispatchType::PropertyGet, +[] { return global::config.get_bg_image_path().wstring(); });
			register_member(L"backgroundImageOpacity", DispatchType::PropertyGet, +[]() { return global::config.get_bg_image_opacity(); });
			register_member(L"curveResolution", DispatchType::PropertyGet, +[] { return global::config.get_curve_resolution(); });
			register_member(L"curveColor", DispatchType::PropertyGet, get_curve_color);
			register_member(L"curveThickness", DispatchType::PropertyGet, +[] { return global::config.get_curve_thickness(); });
			register_member(L"notifyUpdate", DispatchType::PropertyGet, +[] { return global::config.get_notify_update(); });
			register_member(L"separatorPos", DispatchType::PropertyGet, +[] { return global::config.get_separator_pos(); });
			register_member(L"separatorPos", DispatchType::PropertyPut, +[](double separator_pos) { global::config.set_separator_pos(separator_pos); });
			register_member(L"presetSize", DispatchType::PropertyGet, +[] { return global::config.get_preset_size(); });
			register_member(L"presetSize", DispatchType::PropertyPut, +[](int preset_size) { global::config.set_preset_size(preset_size); });
			register_member(L"applyButtonHeight", DispatchType::PropertyGet, +[] { return global::config.get_apply_button_height(); });
		}
	};
} // namespace curve_editor