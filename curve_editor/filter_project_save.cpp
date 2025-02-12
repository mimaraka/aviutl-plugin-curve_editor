#include "constants.hpp"
#include "curve_editor.hpp"
#include "filter_project_save.hpp"
#include <cereal/archives/binary.hpp>
#include <sstream>



namespace curve_editor {
	BOOL filter_project_save(AviUtl::FilterPlugin*, AviUtl::EditHandle*, void* data, int32_t* size) {		
		static std::ostringstream oss;

		// まずサイズの取得のためにdataがNULLで呼び出される
		// ここでカーブのデータを作成しておく
		if (!data) {
			oss.str("");
			oss.clear(std::stringstream::goodbit);
			cereal::BinaryOutputArchive archive(oss);
			archive(global::editor);
			*size = static_cast<int32_t>(oss.str().size());
		}
		// dataにデータをコピーする
		else {
			memcpy(data, oss.str().data(), oss.str().size());
		}

		return TRUE;
	}
} // namespace curve_editor