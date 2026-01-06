#pragma once

#include <atomic>
#include <memory>

#include "danejoe/network/runtime/reactor_mail_box.hpp"
#include "protocol/server_message_codec.hpp"
#include "service/server_file_info_service.hpp"

class BusinessRuntime
{
public:
    BusinessRuntime(std::shared_ptr<DaneJoe::ReactorMailBox> reactor_mail_box);
    void init();
    void run();
    void stop();
    void handle_request(
        const std::vector<uint8_t>& data,
        uint64_t connect_id);
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
        int64_t request_id,
        uint64_t connect_id);
    /**
     * @brief 处理测试请求
     * @param request_info 请求信息
     */
    void handle_test_request(
        const TestRequestTransfer& test_request,
        int64_t request,
        uint64_t connect_id);
    /**
     * @brief 处理块请求
     * @param request_info 请求信息
     */
    void handle_block_request(
        const BlockRequestTransfer& block_request,
        int64_t request_id,
        uint64_t connect_id);
private:
    std::atomic<bool> m_is_running = false;
    std::shared_ptr<DaneJoe::ReactorMailBox> m_reactor_mail_box = nullptr;
    ServerMessageCodec m_message_codec;
    ServerFileInfoService m_file_info_service;
};