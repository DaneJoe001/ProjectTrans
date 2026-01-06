#pragma once

#include <cstdint>
#include <optional>
#include <mutex>
#include <queue>
#include <unordered_map>

#include "danejoe/concurrent/container/mpmc_bounded_queue.hpp"
#include "danejoe/network/container/posix_frame.hpp"
#include "danejoe/network/handle/posix_event_handle.hpp"

namespace DaneJoe
{
    class ReactorMailBox
    {
    public:
        ReactorMailBox();
        ~ReactorMailBox();
        void set_event_handle(std::shared_ptr<PosixEventHandle> event_handle);
        void add_to_client_queue(uint64_t connect_id);
        void remove_to_client_queue(uint64_t connect_id);
        void push_to_client_frame(const PosixFrame& frame);
        void push_to_server_frame(const PosixFrame& frame);
        void push_to_server_frame(const std::vector<PosixFrame>& frame);

        std::optional<PosixFrame> pop_from_to_server_frame();
        std::optional<PosixFrame> try_pop_from_to_server_queue();
        std::optional<PosixFrame> pop_from_to_client_queue(
            uint64_t connect_id);
        void stop();
    private:
        std::shared_ptr<PosixEventHandle> m_event_handle = nullptr;
        std::mutex m_client_queues_mutex;
        /// @brief IO接收的来自客户端，待交给业务线程处理的帧序列
        MpmcBoundedQueue<PosixFrame> m_to_server_frame_queue;
        std::unordered_map<uint64_t, std::queue<PosixFrame>> m_to_client_queues;

    };
}