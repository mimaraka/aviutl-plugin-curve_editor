#include "util.hpp"
#include <regex>
#include "constants.hpp"
#include "global.hpp"



namespace cved {
	namespace util {
		// 長さを減算
		void get_cutoff_line(mkaul::Point<float>* pt, const mkaul::Point<float>& start, const mkaul::Point<float>& end, float length) {
			float old_length = (float)mkaul::distance(start, end);

			if (old_length == 0) {
				pt->x = end.x;
				pt->y = end.y;
				return;
			}
			if (length > old_length) {
				pt->x = start.x;
				pt->y = start.y;
				return;
			}

			double length_ratio = (old_length - length) / old_length;
			float after_x = (float)(start.x + (end.x - start.x) * length_ratio);
			float after_y = (float)(start.y + (end.y - start.y) * length_ratio);

			pt->x = after_x;
			pt->y = after_y;
		}

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
			auto tra_script_names = reinterpret_cast<const char**>(global::exedit_internal.base_address() + 0x231488u);
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