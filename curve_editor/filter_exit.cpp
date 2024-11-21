#include "config.hpp"
#include "filter_exit.hpp"
#include "preset_manager.hpp"
#include <mkaul/graphics.hpp>



namespace curve_editor {
	BOOL filter_exit(AviUtl::FilterPlugin*) {
		// 設定・プリセットをJSONに保存
		global::config.save_json();
		global::preset_manager.save_json();

		return TRUE;
	}
} // namespace curve_editor