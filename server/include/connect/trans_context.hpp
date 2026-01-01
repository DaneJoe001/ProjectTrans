#pragma once

/**
 * @file trans_context.hpp
 * @brief 传输上下文
 * @author DaneJoe001
 */

#include <cstdint>

#include <danejoe/concurrent/container/mpmc_bounded_queue.hpp>
#include "danejoe/network/context/i_socket_context.hpp"
#include "danejoe/network/codec/frame_assembler.hpp"

#include "model/transfer/block_transfer.hpp"
#include "model/transfer/download_transfer.hpp"
#include "model/transfer/test_transfer.hpp"
#include "protocol/server_message_codec.hpp"
#include "service/server_file_info_service.hpp"

 /**
  * @class TransContext
  * @brief 传输上下文
  */
class TransContext : public DaneJoe::ISocketContext
{
public:
    /**
     * @brief 构造函数
     */
    TransContext();
    /**
     * @brief 接收数据回调
     * @param buffer 接收到的数据
     */
    void on_recv()override;
    /**
     * @brief 发送数据回调
     * @param buffer 发送的数据
     */
    void on_send()override;
    /**
     * @brief 销毁传输上下文，当前仅执行解注册
     */
    void destroy(std::shared_ptr<ISocketContext> context);
    void handle_request(const std::vector<uint8_t>& data);
    /**
     * @brief 处理未知请求
     * @param request_info 请求信息
     */
    void handle_unknown_request();
    /**
     * @brief 处理下载请求
     * @param request_info 请求信息
     */
    void handle_download_request(
        const DownloadRequestTransfer& download_request,
        int64_t request_id);
    /**
     * @brief 处理测试请求
     * @param request_info 请求信息
     */
    void handle_test_request(const TestRequestTransfer& test_request, int64_t request);
    /**
     * @brief 处理块请求
     * @param request_info 请求信息
     */
    void handle_block_request(const BlockRequestTransfer& block_request, int64_t request_id);
private:
    ServerMessageCodec m_message_codec;
    /// @brief 帧组装器
    DaneJoe::FrameAssembler m_frame_assembler;
    /// @brief 文件信息服务
    ServerFileInfoService m_file_info_service;
};

/**
 * @class TransContextCreator
 * @brief 传输上下文创建者
 */
class TransContextCreator : public DaneJoe::ISocketContextCreator
{
public:
    /**
     * @brief 创建传输上下文
     * @return 传输上下文
     */
    std::shared_ptr<DaneJoe::ISocketContext> create()override;
    /**
     * @brief 创建传输上下文
     * @param recv_buffer 接收缓冲区
     * @param send_buffer 发送缓冲区
     * @return 传输上下文
     */
    std::shared_ptr<DaneJoe::ISocketContext> create(
        std::shared_ptr<DaneJoe::MpmcBoundedQueue<uint8_t>> recv_buffer,
        std::shared_ptr<DaneJoe::MpmcBoundedQueue<uint8_t>> send_buffer)override;
    /**
     * @brief 销毁传输上下文
     * @param context 传输上下文
     */
    void destroy(std::shared_ptr<DaneJoe::ISocketContext> context)override;
};