/**
 * @file view_event_hub.hpp
 * @author DaneJoe001 (danejoe001.github)
 * @brief 视图事件中心类
 * @date 2025-12-28
 */
#pragma once

#include <atomic>

#include <QObject>

#include "common/protocol/network_endpoint.hpp"
#include "view/event/event_envelope.hpp"
#include "model/transfer/test_transfer.hpp"
#include "model/transfer/block_transfer.hpp"
#include "model/transfer/download_transfer.hpp"
#include "model/entity/task_entity.hpp"
#include "context/trans_context.hpp"

 /**
  * @class ViewEventHub
  * @brief 视图事件中心类
  * @details 负责视图层事件的发布和分发，采用发布-订阅模式。为事件生成唯一ID和时间戳，
  *          封装成事件封包后通过Qt信号机制分发
  */
class ViewEventHub : public QObject
{
    Q_OBJECT
public:
    /**
     * @brief 初始化事件中心
     * @details 执行事件中心的初始化操作
     */
    void init();
    /**
     * @brief 发布测试请求事件
     * @param event_source 事件源上下文，标识事件的发起者
     * @param endpoint 网络端点，指定请求的目标地址和端口
     * @param request 测试请求传输对象
     * @details 创建事件封包，生成唯一事件ID和时间戳，发出测试请求信号
     */
    void publish_test_request(
        EventContext event_source,
        NetworkEndpoint endpoint,
        TestRequestTransfer request);
    /**
     * @brief 发布块请求事件
     * @param event_source 事件源上下文，标识事件的发起者
     * @param endpoint 网络端点，指定请求的目标地址和端口
     * @param request 块请求传输对象
     * @details 创建事件封包，生成唯一事件ID和时间戳，发出块请求信号
     */
    void publish_block_request(
        EventContext event_source,
        NetworkEndpoint endpoint,
        BlockRequestTransfer request);
    /**
     * @brief 发布下载请求事件
     * @param event_source 事件源上下文，标识事件的发起者
     * @param endpoint 网络端点，指定请求的目标地址和端口
     * @param request 下载请求传输对象
     * @details 创建事件封包，生成唯一事件ID和时间戳，发出下载请求信号
     */
    void publish_download_request(
        EventContext event_source,
        NetworkEndpoint endpoint,
        DownloadRequestTransfer request);
    /**
     * @brief 发布测试响应事件
     * @param event_envelope 事件封包，包含原始请求的事件信息
     * @param trans_context 传输上下文，包含请求ID和端点信息
     * @param response 测试响应传输对象
     * @details 直接发出测试响应信号，用于响应匹配和事件追踪
     */
    void publish_test_response(EventEnvelope event_envelope,
        TransContext trans_context,
        TestResponseTransfer response);
    /**
     * @brief 发布下载响应事件
     * @param event_envelope 事件封包，包含原始请求的事件信息
     * @param trans_context 传输上下文，包含请求ID和端点信息
     * @param response 下载响应传输对象
     * @details 直接发出下载响应信号，用于响应匹配和事件追踪
     */
    void publish_download_response(EventEnvelope event_envelope,
        TransContext trans_context,
        DownloadResponseTransfer response);
    /**
     * @brief 发布块响应事件
     * @param event_envelope 事件封包，包含原始请求的事件信息
     * @param trans_context 传输上下文，包含请求ID和端点信息
     * @param response 块响应传输对象
     * @details 直接发出块响应信号，用于响应匹配和事件追踪
     */
    void publish_block_response(EventEnvelope event_envelope,
        TransContext trans_context,
        BlockResponseTransfer response);
    void publish_task_entity_update(int64_t task_id);
    void publish_task_entity_add(TaskEntity task_entity);
signals:
    void task_entity_update(int64_t task_id);
    void task_entity_add(TaskEntity task_entity);
    /**
     * @brief 测试请求信号
     * @param event_envelope 事件封包，包含事件ID、上下文和时间戳
     * @param endpoint 网络端点，指定请求的目标地址和端口
     * @param request 测试请求传输对象
     */
    void test_request(
        EventEnvelope event_envelope,
        NetworkEndpoint endpoint,
        TestRequestTransfer request);
    /**
     * @brief 块请求信号
     * @param event_envelope 事件封包，包含事件ID、上下文和时间戳
     * @param endpoint 网络端点，指定请求的目标地址和端口
     * @param request 块请求传输对象
     */
    void block_request(
        EventEnvelope event_envelope,
        NetworkEndpoint endpoint,
        BlockRequestTransfer request);
    /**
     * @brief 下载请求信号
     * @param event_envelope 事件封包，包含事件ID、上下文和时间戳
     * @param endpoint 网络端点，指定请求的目标地址和端口
     * @param request 下载请求传输对象
     */
    void download_request(
        EventEnvelope event_envelope,
        NetworkEndpoint endpoint,
        DownloadRequestTransfer request);
    /**
     * @brief 测试响应信号
     * @param event_envelope 事件封包，包含原始请求的事件信息
     * @param trans_context 传输上下文，包含请求ID和端点信息
     * @param response 测试响应传输对象
     */
    void test_response(
        EventEnvelope event_envelope,
        TransContext trans_context,
        TestResponseTransfer response);
    /**
     * @brief 下载响应信号
     * @param event_envelope 事件封包，包含原始请求的事件信息
     * @param trans_context 传输上下文，包含请求ID和端点信息
     * @param response 下载响应传输对象
     */
    void download_response(
        EventEnvelope event_envelope, 
        TransContext trans_context,
        DownloadResponseTransfer response);
    /**
     * @brief 块响应信号
     * @param event_envelope 事件封包，包含原始请求的事件信息
     * @param trans_context 传输上下文，包含请求ID和端点信息
     * @param response 块响应传输对象
     */
    void block_response(
        EventEnvelope event_envelope, 
        TransContext trans_context,
        BlockResponseTransfer response);
private:
    /// @brief 事件ID计数器，用于生成唯一的事件ID
    std::atomic<uint64_t> m_event_id_counter = 0;
};