#include "config.hpp"
#include "filter_exit.hpp"
#include <mkaul/graphics.hpp>



namespace cved {
	BOOL filter_exit(AviUtl::FilterPlugin*) {
		// 設定をJSONに保存
		global::config.save_json();
		//mkaul::graphics::Factory::shutdown();

		return TRUE;
	}
}