#include "filter_project_load.hpp"
#include "actctx_manager.hpp"
#include "constants.hpp"
#include "curve_editor.hpp"
#include "global.hpp"
#include "my_messagebox.hpp"
#include "string_table.hpp"



namespace cved {
	BOOL filter_project_load(AviUtl::FilterPlugin* fp, AviUtl::EditHandle*, void* data, int32_t size) {
		using StringId = global::StringTable::StringId;
		constexpr size_t DATA_SIZE_V1 = 1839104u;
		const size_t PREFIX_N = strlen(global::DATA_PREFIX);
		BOOL ret = FALSE;

		if (data) {
			// IDカーブのリセット
			global::editor.reset_id_curves();
			// sizeがプレフィックス+バージョン識別子のサイズより大きいことを確認
			if (PREFIX_N <= size) {
				// プレフィックスを読み込む
				auto bytes_data = reinterpret_cast<byte*>(data);
				auto str_prefix = std::string{ bytes_data, bytes_data + PREFIX_N };

				// プレフィックスが"CEV2"の場合
				if (str_prefix == global::DATA_PREFIX) {
					if (global::editor.load_data(bytes_data + PREFIX_N, size - PREFIX_N)) {
						ret = TRUE;
					}
				}
				// v1.xのデータの場合
				if (!ret and size == DATA_SIZE_V1) {
					if (global::editor.editor_graph().load_data_v1(bytes_data)) {
						ret = TRUE;
					}
				}
			}
			// ウィンドウの更新
			global::window_main.send_command((WPARAM)WindowCommand::Update);

			// データの読み込みに失敗した場合
			if (!ret) {
				ActCtxManager actctx_manager;
				actctx_manager.init(fp->dll_hinst);
				my_messagebox(
					global::string_table[StringId::ErrorDataLoadFailed],
					fp->hwnd,
					MessageBoxIcon::Error
				);
				actctx_manager.exit();
			}
		}
		return ret;
	}
}