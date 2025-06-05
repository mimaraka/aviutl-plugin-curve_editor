#include "constants.hpp"
#include "global.hpp"
#include "util.hpp"
#include <regex>



namespace curve_editor::util {
	// クリップボードに文字列をコピー
	bool copy_to_clipboard(HWND hwnd, const std::wstring& str) noexcept {
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
		memory = ::GlobalAlloc(GHND | GMEM_SHARE, sizeof(wchar_t) * (str.length() + 1));

		if (memory) {
			// メモリブロックへのアクセスを開始
			buffer = static_cast<LPTSTR>(::GlobalLock(memory));
			if (buffer) {
				// 文字列をコピー
				::lstrcpyW(buffer, str.data());
				// メモリブロックへのアクセスを終了
				::GlobalUnlock(memory);
				::SetClipboardData(CF_UNICODETEXT, memory);
				ret = true;
			}
		}
		::CloseClipboard();

		return ret;
	}
} // namespace curve_editor::util