#pragma once

#include <Windows.h>



namespace cved {
    class ActCtxHelper {
        ULONG_PTR cookie_ = 0ul;
        HANDLE hactctx_ = NULL;

    public:
        bool init() noexcept;
        bool exit();
    };
}