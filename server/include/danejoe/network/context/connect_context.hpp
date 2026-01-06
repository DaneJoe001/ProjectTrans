#pragma once

#include "danejoe/network/handle/posix_socket_handle.hpp"
#include "danejoe/network/codec/frame_assembler.hpp"
#include "danejoe/common/result/result.hpp"
#include "danejoe/network/container/posix_frame.hpp"

namespace DaneJoe
{
    class ConnectContext
    {
    public:
        ConnectContext(
            uint64_t connect_id,
            PosixSocketHandle&& socket_handle);
        Result<std::vector<PosixFrame>> read();
        Result<int> write(std::vector<PosixFrame> frames);
        bool has_pending_write() const;
        uint64_t get_connect_id()const;
    private:
        const int BUFFER_SIZE = 1024;
        uint64_t m_connect_id = 0;
        PosixSocketHandle m_socket_handle;
        FrameAssembler m_frame_assembler;
        Buffer m_write_buffer;
    };
}