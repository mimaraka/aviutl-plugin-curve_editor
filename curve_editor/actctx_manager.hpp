#pragma once

#include <Windows.h>



namespace cved {
    class ActCtxManager {
        ULONG_PTR cookie_ = 0ul;
        HANDLE hactctx_ = NULL;

    public:
        bool init(HINSTANCE hinst) noexcept;
        bool exit();
    };
}