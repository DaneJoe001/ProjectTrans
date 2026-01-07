/**
 * @file view_event_controller.hpp
 * @author DaneJoe001 (danejoe001.github)
 * @brief 事件控制器
 * @date 2025-12-28
 */
#pragma once

#include <unordered_map>

#include <QObject>
#include <QPointer>

#include "common/protocol/network_endpoint.hpp"
#include "view/event/event_envelope.hpp"
#include "view/event/view_event_hub.hpp"
#include "model/transfer/test_transfer.hpp"
#include "model/transfer/block_transfer.hpp"
#include "model/transfer/download_transfer.hpp"
#include "service/trans_service.hpp"

 /**
  * @class ViewEventController
  * @brief 事件控制器
  * @details 负责在 ViewEventHub 与 TransService 之间建立桥接：
  *          - 接收来自视图层的请求事件并转发为网络请求
  *          - 接收来自传输层的响应信号并回投到事件中心
  *          - 维护 request_id -> EventEnvelope 的关联，用于请求-响应匹配
  */
class ViewEventController : public QObject
{
    Q_OBJECT
public:
    /**
     * @brief 构造函数
     * @param event_hub 事件中心
     * @param trans_service 传输服务
     * @param parent 父对象
     */
    ViewEventController(
        QPointer<ViewEventHub> event_hub,
        TransService& trans_service,
        QObject* parent = nullptr);
    /**
     * @brief 初始化控制器
     * @details 执行控制器的初始化操作，建立事件中心与传输服务之间的连接
     */
    void init();
signals:
public slots:
    /**
     * @brief 处理测试请求事件
     * @param event_envelope 事件封包，包含事件ID、上下文和时间戳
     * @param endpoint 网络端点，指定请求的目标地址和端口
     * @param request 测试请求传输对象，包含请求的具体消息内容
     * @details 当接收到测试请求信号时，通过传输服务发送请求到指定端点，
     *          并将事件封包与请求ID关联存储，用于后续响应匹配
     */
    void on_test_request(
        EventEnvelope event_envelope,
        NetworkEndpoint endpoint,
        TestRequestTransfer request);
    /**
     * @brief 处理块请求事件
     * @param event_envelope 事件封包，包含事件ID、上下文和时间戳
     * @param endpoint 网络端点，指定请求的目标地址和端口
     * @param request 块请求传输对象，包含块请求的具体信息
     * @details 当接收到块请求信号时，通过传输服务发送块请求到指定端点，
     *          并将事件封包与请求ID关联存储，用于后续响应匹配和状态追踪
     */
    void on_block_request(
        EventEnvelope event_envelope,
        NetworkEndpoint endpoint,
        BlockRequestTransfer request);
    /**
     * @brief 处理下载请求事件
     * @param event_envelope 事件封包，包含事件ID、上下文和时间戳
     * @param endpoint 网络端点，指定下载请求的目标地址和端口
     * @param request 下载请求传输对象，包含下载任务的具体信息
     * @details 当接收到下载请求信号时，通过传输服务发送下载请求到指定端点，
     *          并将事件封包与请求ID关联存储，用于后续响应匹配和下载进度追踪
     */
    void on_download_request(
        EventEnvelope event_envelope,
        NetworkEndpoint endpoint,
        DownloadRequestTransfer request);
    /**
     * @brief 处理测试响应事件
     * @param trans_context 传输上下文，包含请求ID等传输相关信息
     * @param response 测试响应传输对象，包含服务器返回的响应消息
     * @details 当接收到测试响应时，根据传输上下文中的请求ID查找对应的原始事件封包，
     *          如果找到匹配的事件，则通过事件中心发布测试响应信号，完成请求-响应的闭环
     */
    void on_test_response(
        TransContext trans_context,
        TestResponseTransfer response);
    /**
     * @brief 处理下载响应事件
     * @param trans_context 传输上下文，包含请求ID等传输相关信息
     * @param response 下载响应传输对象，包含服务器返回的响应消息
     * @details 当接收到下载响应时，根据传输上下文中的请求ID查找对应的原始事件封包，
     *          如果找到匹配的事件，则通过事件中心发布下载响应信号，完成请求-响应的闭环
     */
    void on_download_response(
        TransContext trans_context,
        DownloadResponseTransfer response);
    /**
     * @brief 处理块响应事件
     * @param trans_context 传输上下文，包含请求ID等传输相关信息
     * @param response 块响应传输对象，包含服务器返回的响应消息
     * @details 当接收到块响应时，根据传输上下文中的请求ID查找对应的原始事件封包，
     *          如果找到匹配的事件，则通过事件中心发布块响应信号，完成请求-响应的闭环
     */
    void on_block_response(
        TransContext trans_context,
        BlockResponseTransfer response);
private:
    /// @brief 事件中心（用于订阅请求事件与发布响应事件）
    QPointer<ViewEventHub> m_view_event_hub;
    /// @brief 传输服务引用（用于发送请求/接收响应）
    TransService& m_trans_service;
    /// @brief 请求ID到事件封包的映射（用于响应到来时匹配原始事件）
    std::unordered_map<uint64_t, EventEnvelope> m_event_envelopes;
};