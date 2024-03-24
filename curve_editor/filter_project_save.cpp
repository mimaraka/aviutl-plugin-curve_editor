#include "filter_project_save.hpp"
#include "constants.hpp"
#include "curve_editor.hpp"



namespace cved {
	BOOL filter_project_save(AviUtl::FilterPlugin*, AviUtl::EditHandle*, void* data, int32_t* size) {		
		static std::vector<byte> curve_data;

		// まずサイズの取得のためにdataがNULLで呼び出される
		// ここでカーブのデータを作成しておく
		if (!data) {
			global::editor.create_data(curve_data);
			*size = curve_data.size();
		}
		// dataにデータをコピーする
		else {
			memcpy(data, curve_data.data(), curve_data.size());
		}

		return TRUE;
	}
}