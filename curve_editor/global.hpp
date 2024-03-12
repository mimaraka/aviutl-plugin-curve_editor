#pragma once

#include <aviutl.hpp>
#include <mkaul/include/window.hpp>
#include <mkaul/include/exedit.hpp>
#include "curve.hpp"
#include "theme.hpp"


namespace cved {
	namespace global {
		inline AviUtl::FilterPlugin* fp = nullptr;
		inline AviUtl::EditHandle* editp = nullptr;
		inline mkaul::exedit::Internal exedit_internal;

		inline mkaul::ui::Window window_main;
		inline mkaul::ui::Window window_toolbar;
		inline mkaul::ui::Window window_editor;
		inline mkaul::ui::Window window_grapheditor;
		inline mkaul::ui::Window window_texteditor;
		inline mkaul::ui::Window window_library;
	}
}