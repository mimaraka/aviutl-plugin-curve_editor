#pragma once

#include <mkaul/include/version.hpp>


namespace cved {
	// Curve Editorの最新バージョンを取得
	bool fetch_latest_version(mkaul::Version* p_version);

	// ::CreateThread()で実行用の関数
	DWORD WINAPI check_for_updates(LPVOID param);
}