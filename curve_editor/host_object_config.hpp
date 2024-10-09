#pragma once

#include <mkaul/host_object.hpp>
#include "config.hpp"



namespace cved {
	class ConfigHostObject : public mkaul::wv2::HostObject {
		static std::wstring get_curve_color();
		static bool is_latest_version(std::wstring latest_version_str);

	public:
		ConfigHostObject() {
			register_member(L"editMode", DispatchType::PropertyGet, +[] { return global::config.get_edit_mode(); });
			register_member(L"editMode", DispatchType::PropertyPut, +[](int edit_mode) { global::config.set_edit_mode((EditMode)edit_mode); });
			register_member(L"editModeNum", DispatchType::PropertyGet, +[] { return (uint32_t)EditMode::NumEditMode; });
			register_member(L"getEditModeName", DispatchType::Method, +[](int edit_mode) { return global::config.get_edit_mode_str((EditMode)edit_mode); });
			register_member(L"showXLabel", DispatchType::PropertyGet, +[] {return global::config.get_show_x_label(); });
			register_member(L"showYLabel", DispatchType::PropertyGet, +[] { return global::config.get_show_y_label(); });
			register_member(L"showHandle", DispatchType::PropertyGet, +[] { return global::config.get_show_handle(); });
			register_member(L"alignHandle", DispatchType::PropertyGet, +[] { return global::config.get_align_handle(); });
			register_member(L"showVelocityGraph", DispatchType::PropertyGet, +[] { return global::config.get_show_velocity_graph(); });
			register_member(L"enableAnimation", DispatchType::PropertyGet, +[] { return global::config.get_enable_animation(); });
			register_member(L"setBackgroundImage", DispatchType::PropertyGet, +[] { return global::config.get_set_bg_image(); });
			register_member(L"backgroundImagePath", DispatchType::PropertyGet, +[] { return global::config.get_bg_image_path().wstring(); });
			register_member(L"backgroundImageOpacity", DispatchType::PropertyGet, +[]() { return global::config.get_bg_image_opacity(); });
			register_member(L"curveResolution", DispatchType::PropertyGet, +[] { return global::config.get_curve_resolution(); });
			register_member(L"curveColor", DispatchType::PropertyGet, get_curve_color);
			register_member(L"notifyUpdate", DispatchType::PropertyGet, +[] { return global::config.get_notify_update(); });
			// TODO: configに載せるべきか？
			register_member(L"isLatestVersion", DispatchType::Method, is_latest_version);
		}
	};
} // namespace cved