#include "actctx_helper.hpp"
#include "constants.hpp"
#include "curve_editor.hpp"
#include "filter_project_load.hpp"
#include "my_webview2_reference.hpp"
#include "my_messagebox.hpp"
#include "string_table.hpp"
#include <cereal/archives/binary.hpp>
#include <sstream>



namespace cved {
	BOOL filter_project_load(AviUtl::FilterPlugin* fp, AviUtl::EditHandle*, void* data, int32_t size) {
		using StringId = global::StringTable::StringId;
		constexpr size_t DATA_SIZE_V1 = 1839104u;
		BOOL ret = FALSE;

		if (data) {
			// IDカーブのリセット
			global::editor.reset_id_curves();

			// v1.xのデータの読み込みを試みる
			if (size == DATA_SIZE_V1) {
				auto bytes_data = static_cast<byte*>(data);
				ret = (BOOL)global::editor.editor_graph().load_v1_data(bytes_data);
			}

			if (!ret) {
				try {
					std::istringstream stream{ std::string{ static_cast<char*>(data), static_cast<size_t>(size)} };
					cereal::BinaryInputArchive archive{ stream };
					archive(global::editor);
					ret = TRUE;
				}
				catch (const std::exception&) {
					global::editor.reset_id_curves();
					ActCtxHelper actctx_helper;
					actctx_helper.init(fp->dll_hinst);
					my_messagebox(
						global::string_table[StringId::ErrorDataLoadFailed],
						fp->hwnd,
						MessageBoxIcon::Error
					);
					actctx_helper.exit();
				}
			}
			// ウィンドウの更新
			if (global::webview) global::webview->post_message(L"updateEditor");
		}
		return ret;
	}
}