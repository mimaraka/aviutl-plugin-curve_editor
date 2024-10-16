#include "constants.hpp"
#include "global.hpp"
#include "util.hpp"
#include <regex>



namespace cved {
	namespace util {
		// クリップボードに文字列をコピー
		bool copy_to_clipboard(HWND hwnd, const char* str) noexcept {
			// 確保するメモリのハンドル
			HGLOBAL memory = nullptr;
			// 確保したメモリの先頭のポインタ
			LPTSTR buffer = nullptr;
			bool ret = false;

			if (!::OpenClipboard(hwnd)) return false;

			// クリップボードを空にする
			::EmptyClipboard();
			// メモリブロックの確保
			memory = ::GlobalAlloc(GHND | GMEM_SHARE, strlen(str) + 1);

			if (memory) {
				// メモリブロックへのアクセスを開始
				buffer = (LPTSTR)::GlobalLock(memory);
				if (buffer) {
					// 文字列をコピー
					::lstrcpyA(buffer, str);
					// メモリブロックへのアクセスを終了
					::GlobalUnlock(memory);
					::SetClipboardData(CF_TEXT, memory);
					ret = true;
				}
			}
			::CloseClipboard();

			return ret;
		}

		int16_t get_track_script_idx() noexcept {
			const std::regex regex_script_name{ std::format(R"(^Type1@{}(\x01.+)?$)", global::PLUGIN_NAME) };
			auto tra_script_names = global::exedit_internal.get<const char*>(0x231488u);
			int16_t script_idx = 0;

			while (true) {
				if (!tra_script_names[script_idx]) return -1;
				if (std::regex_match(tra_script_names[script_idx], regex_script_name)) {
					break;
				}
				script_idx++;
			}
			return script_idx;
		}
	}
}