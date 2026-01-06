#pragma once

#include <cstdint>

#include "danejoe/common/result/result.hpp"
#include "danejoe/common/handle/unique_handle.hpp"
#include "danejoe/common/type_traits/platform_traits.hpp"

#if DANEJOE_PLATFORM_LINUX==1
namespace DaneJoe
{
    class PosixEventHandle
    {
    public:
        PosixEventHandle();
        PosixEventHandle(uint32_t count, int flags);
        operator bool();
        void init(uint32_t count, int flags);
        Result<uint64_t> read();
        Result<int> write(uint64_t value);
        const UniqueHandle<int>& get_handle()const;
    private:
        UniqueHandle<int> m_handle;
    };
}
#endif