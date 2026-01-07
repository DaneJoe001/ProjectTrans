/**
 * @file reactor_mail_box.hpp
 * @author DaneJoe (danejoe001.github)
 * @brief Reactor 邮箱
 * @version 0.2.0
 * @date 2026-01-06
 * @details 定义 ReactorMailBox，用于在网络 IO 线程与业务线程之间传递帧数据。
 *          邮箱通常包含两类通道：
 *          - to_server：IO 线程接收客户端数据后投递给业务线程处理
 *          - to_client：业务线程生成响应帧后投递给 IO 线程发送
 *          当设置了 PosixEventHandle 时，邮箱可通过写入通知值唤醒事件循环。
 */
#pragma once

#include <cstdint>
#include <optional>
#include <mutex>
#include <queue>
#include <unordered_map>

#include "danejoe/concurrent/container/mpmc_bounded_queue.hpp"
#include "danejoe/network/container/posix_frame.hpp"
#include "danejoe/network/handle/posix_event_handle.hpp"

/**
 * @namespace DaneJoe
 * @brief DaneJoe 命名空间
 */
namespace DaneJoe
{
    /**
     * @class ReactorMailBox
     * @brief Reactor 邮箱
     * @details 用于跨线程传递 PosixFrame：
     *          - push_to_server_frame()/pop_from_to_server_frame()：客户端 -> 业务线程
     *          - push_to_client_frame()/pop_from_to_client_queue()：业务线程 -> IO 线程
     *
     *          其中 to_server 使用 MPMC 有界队列以支持多生产者/多消费者并限制内存增长；
     *          to_client 使用按 connect_id 划分的队列，以便按连接组织待发送帧。
     *
     * @note 线程安全：
     *       - to_server 队列由 MpmcBoundedQueue 保证并发安全
     *       - to_client 队列的创建/销毁与访问通常需要互斥保护（见 m_client_queues_mutex）
     */
    class ReactorMailBox
    {
    public:
        /**
         * @brief 默认构造
         */
        ReactorMailBox();
        /**
         * @brief 析构
         */
        ~ReactorMailBox();
        /**
         * @brief 设置通知事件句柄
         * @param event_handle 通知事件句柄（用于唤醒事件循环）
         */
        void set_event_handle(std::shared_ptr<PosixEventHandle> event_handle);
        /**
         * @brief 为指定连接创建 to_client 队列
         * @param connect_id 连接标识
         */
        void add_to_client_queue(uint64_t connect_id);
        /**
         * @brief 移除指定连接的 to_client 队列
         * @param connect_id 连接标识
         */
        void remove_to_client_queue(uint64_t connect_id);
        /**
         * @brief 投递待发送帧（业务线程 -> IO 线程）
         * @param frame 待发送帧
         */
        void push_to_client_frame(const PosixFrame& frame);
        /**
         * @brief 投递待处理帧（IO 线程 -> 业务线程）
         * @param frame 待处理帧
         */
        void push_to_server_frame(const PosixFrame& frame);
        /**
         * @brief 批量投递待处理帧（IO 线程 -> 业务线程）
         * @param frame 待处理帧集合
         */
        void push_to_server_frame(const std::vector<PosixFrame>& frame);

        /**
         * @brief 阻塞弹出一个待处理帧
         * @return 若成功获得帧则返回该帧；否则返回 std::nullopt
         */
        std::optional<PosixFrame> pop_from_to_server_frame();
        /**
         * @brief 非阻塞尝试弹出一个待处理帧
         * @return 若当前有帧则返回该帧，否则返回 std::nullopt
         */
        std::optional<PosixFrame> try_pop_from_to_server_queue();
        /**
         * @brief 从指定连接的 to_client 队列中弹出一个待发送帧
         * @param connect_id 连接标识
         * @return 若存在待发送帧则返回该帧，否则返回 std::nullopt
         */
        std::optional<PosixFrame> pop_from_to_client_queue(
            uint64_t connect_id);
        /**
         * @brief 停止邮箱
         * @details 通常用于通知内部队列退出阻塞等待并结束消费循环。
         */
        void stop();
    private:
        /// @brief 通知事件句柄（用于唤醒事件循环）
        std::shared_ptr<PosixEventHandle> m_event_handle = nullptr;
        /// @brief to_client 队列集合的互斥锁
        std::mutex m_client_queues_mutex;
        /// @brief IO 线程接收来自客户端、待交给业务线程处理的帧队列
        MpmcBoundedQueue<PosixFrame> m_to_server_frame_queue;
        /// @brief 业务线程投递给 IO 线程、按连接划分的待发送帧队列
        std::unordered_map<uint64_t, std::queue<PosixFrame>> m_to_client_queues;

    };
}