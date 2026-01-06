#pragma once

#include <cstddef>

#include "danejoe/network/container/buffer.hpp"
#include "danejoe/common/type_traits/platform_traits.hpp"
#include "danejoe/common/handle/unique_handle.hpp"
#include "danejoe/common/result/result.hpp"

#if DANEJOE_PLATFORM_LINUX==1
#include <sys/socket.h>
#endif

namespace DaneJoe
{
#if DANEJOE_PLATFORM_LINUX==1
    class PosixSocketHandle
    {
    public:
        PosixSocketHandle();
        PosixSocketHandle(int domain, int type, int protocol);
        PosixSocketHandle(int other);
        operator bool();
        Result<Buffer> read(std::size_t size);
        Result<Buffer> read_some(std::size_t size);
        Result<std::size_t> write(DaneJoe::Buffer buffer);
        Result<std::size_t> write_some(DaneJoe::Buffer buffer);
        Result<std::size_t> write_some(uint8_t* buffer, std::size_t size);
        StatusCode connect(const sockaddr* address, socklen_t length);
        StatusCode bind(const sockaddr* address, socklen_t length);
        StatusCode listen(int nums);
        Result<PosixSocketHandle> accept(sockaddr* address = nullptr, socklen_t* length = nullptr, int flags = 0);
        StatusCode set_blocking(bool is_blocking);
        const UniqueHandle<int>& get_handle()const;
    private:
        bool m_is_blocking = false;
        UniqueHandle<int> m_handle;
    };
#endif
};