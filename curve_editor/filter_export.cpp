#include <format>
#include <aviutl.hpp>
#include "constants.hpp"
#include "func_filter.hpp"



auto __stdcall GetFilterTable()
{
	using Flag = AviUtl::FilterPluginDLL::Flag;
	static auto str_info = std::format("{} {}", cved::global::PLUGIN_NAME, cved::global::PLUGIN_VERSION.str());

	static AviUtl::FilterPluginDLL filter = {
		.flag = Flag::AlwaysActive
				| Flag::WindowSize
				| Flag::WindowThickFrame
				| Flag::DispFilter
				| Flag::ExInformation,
		.x = 320,
		.y = 540,
		.name = cved::global::PLUGIN_NAME,
		.func_init = cved::filter_init,
		.func_exit = cved::filter_exit,
		.func_WndProc = cved::filter_wndproc,
		.information = str_info.c_str(),
		.func_project_load = cved::filter_project_load,
		.func_project_save = cved::filter_project_save
	};

	return &filter;
}