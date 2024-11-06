#include "constants.hpp"
#include "global.hpp"
#include "util.hpp"
#include <regex>



namespace cved::util {
	// クリップボードに文字列をコピー
	bool copy_to_clipboard(HWND hwnd, const char* str) noexcept {
		// 確保するメモリのハンドル
		HGLOBAL memory = nullptr;
		// 確保したメモリの先頭のポインタ
		LPTSTR buffer = nullptr;
		bool ret = false;

		if (!::OpenClipboard(hwnd)) {
			return false;
		}

		// クリップボードを空にする
		::EmptyClipboard();
		// メモリブロックの確保
		memory = ::GlobalAlloc(GHND | GMEM_SHARE, strlen(str) + 1);

		if (memory) {
			// メモリブロックへのアクセスを開始
			buffer = static_cast<LPTSTR>(::GlobalLock(memory));
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
} // namespace cved::util