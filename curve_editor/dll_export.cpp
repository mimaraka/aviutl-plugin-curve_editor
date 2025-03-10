#include <format>

#include <aviutl.hpp>
#include <strconv2.h>

#include "constants.hpp"
#include "filter_exit.hpp"
#include "filter_init.hpp"
#include "filter_project_load.hpp"
#include "filter_project_save.hpp"
#include "filter_wndproc.hpp"


auto __stdcall GetFilterTable() {
	using Flag = AviUtl::FilterPluginDLL::Flag;
	static auto str_name = ::wide_to_sjis(curve_editor::global::PLUGIN_DISPLAY_NAME.data());
	static auto str_info = std::format("{} {}", ::wide_to_sjis(curve_editor::global::PLUGIN_DISPLAY_NAME.data()), curve_editor::global::PLUGIN_VERSION.str());

	static AviUtl::FilterPluginDLL filter = {
		.flag = Flag::AlwaysActive
				| Flag::WindowSize
				| Flag::WindowThickFrame
				| Flag::DispFilter
				| Flag::ExInformation,
		.x = 550,
		.y = 460,
		.name = str_name.c_str(),
		.func_init = curve_editor::filter_init,
		.func_exit = curve_editor::filter_exit,
		.func_WndProc = curve_editor::filter_wndproc,
		.information = str_info.c_str(),
		.func_project_load = curve_editor::filter_project_load,
		.func_project_save = curve_editor::filter_project_save
	};

	return &filter;
}