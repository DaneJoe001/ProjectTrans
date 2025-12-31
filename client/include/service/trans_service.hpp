/**
 * @file trans_service.hpp
 * @author DaneJoe001 (danejoe001.github)
 * @brief 传输服务类
 * @date 2025-12-28
 */
#pragma once

#include <atomic>
#include <mutex>
#include <vector>
#include <chrono>
#include <cstdint>
#include <functional>
#include <unordered_map>

#include <QObject>
#include <QThread>
#include <QPointer>
#include <QTcpSocket>
#include <QByteArray>

#include "common/protocol/network_endpoint.hpp"
#include "danejoe/concurrent/timer/timer_manager.hpp"

#include "service/network_service.hpp"
#include "protocol/client_message_codec.hpp"
#include "model/transfer/test_transfer.hpp"
#include "model/transfer/download_transfer.hpp"
#include "model/transfer/block_transfer.hpp"
#include "context/trans_context.hpp"

 /**
  * @struct TransCorrelation
  * @brief 传输关联结构
  * @details 用于关联传输上下文和响应回调函数，实现请求-响应的匹配
  */
struct TransCorrelation
{
    /// @brief 传输上下文，包含请求ID和网络端点信息
    TransContext context;
    /// @brief 响应回调函数，当收到响应时调用
    std::function<void(std::vector<uint8_t>)> callback;
};

/**
 * @class TransService
 * @brief 传输服务类
 * @details 负责管理网络传输请求和响应，采用单例模式。提供测试、下载和块请求的发送功能，
 *          并管理请求-响应的关联关系，支持超时处理
 */
class TransService : public QObject
{
    Q_OBJECT
public:
    /**
     * @brief 构造函数
     * @details 私有构造函数，实现单例模式。初始化网络线程和服务，设置超时间隔
     */
    TransService();
    /**
     * @brief 析构函数
     * @details 清理网络服务和线程资源
     */
    ~TransService();
    /**
     * @brief 初始化服务
     * @details 执行服务的初始化操作
     */
    void init();
    /**
     * @brief 发送测试请求
     * @param endpoint 网络端点，指定目标地址和端口
     * @param request 测试请求传输对象
     * @return 传输上下文，包含请求ID和端点信息
     * @details 生成请求ID，注册超时处理，建立请求-响应关联，发送请求到网络服务
     */
    TransContext send_test_request(
        const NetworkEndpoint& endpoint,
        const TestRequestTransfer& request);
    /**
     * @brief 发送下载请求
     * @param endpoint 网络端点，指定目标地址和端口
     * @param request 下载请求传输对象
     * @return 传输上下文，包含请求ID和端点信息
     * @details 生成请求ID，注册超时处理，建立请求-响应关联，发送请求到网络服务
     */
    TransContext send_download_request(
        const NetworkEndpoint& endpoint,
        const DownloadRequestTransfer& request);
    /**
     * @brief 发送块请求
     * @param endpoint 网络端点，指定目标地址和端口
     * @param request 块请求传输对象
     * @return 传输上下文，包含请求ID和端点信息
     * @details 生成请求ID，注册超时处理，建立请求-响应关联，发送请求到网络服务
     */
    TransContext send_block_request(
        const NetworkEndpoint& endpoint,
        const BlockRequestTransfer& request);
signals:
    /**
     * @brief 测试响应接收信号
     * @param trans_context 传输上下文，包含请求ID和端点信息
     * @param response 测试响应传输对象
     */
    void test_response_received(
        TransContext trans_context,
        TestResponseTransfer response);
    /**
     * @brief 下载响应接收信号
     * @param trans_context 传输上下文，包含请求ID和端点信息
     * @param response 下载响应传输对象
     */
    void download_response_received(
        TransContext trans_context,
        DownloadResponseTransfer response);
    /**
     * @brief 块响应接收信号
     * @param trans_context 传输上下文，包含请求ID和端点信息
     * @param response 块响应传输对象
     */
    void block_response_received(
        TransContext trans_context,
        BlockResponseTransfer response);
private:
    /**
     * @brief 接收测试响应
     * @param request_id 请求ID
     * @param data 响应数据
     * @details 解析响应数据，查找对应的传输关联，发出测试响应接收信号
     */
    void receive_test_response(
        TransContext trans_context,
        const std::vector<uint8_t>& data);
    /**
     * @brief 接收下载响应
     * @param request_id 请求ID
     * @param data 响应数据
     * @details 解析响应数据，查找对应的传输关联，发出下载响应接收信号
     */
    void receive_download_response(
        TransContext trans_context,
        const std::vector<uint8_t>& data);
    /**
     * @brief 接收块响应
     * @param request_id 请求ID
     * @param data 响应数据
     * @details 解析响应数据，查找对应的传输关联，发出块响应接收信号
     */
    void receive_block_response(
        TransContext trans_context,
        const std::vector<uint8_t>& data);
    /**
     * @brief 移除响应处理器
     * @param request_id 请求ID
     * @details 从传输关联映射中移除指定请求ID的关联关系
     */
    void remove_response_handler(uint64_t request_id);
signals:
    /**
     * @brief 发送帧就绪信号
     * @param data 要发送的帧数据
     */
    void send_frame_ready(NetworkEndpoint endpoint, QByteArray data);
public slots:
    /**
     * @brief 处理接收到的帧数据
     * @param data 接收到的帧数据
     * @details 解析响应数据，查找对应的响应处理器，执行回调并移除处理器
     */
    void on_received_frame_ready(QByteArray data);
private:
    /// @brief 互斥锁，保护传输关联映射的并发访问
    std::mutex m_mutex;
    /// @brief 超时间隔，请求超时的时间长度
    std::chrono::steady_clock::duration m_time_out_interval;
    /// @brief 定时器管理器引用，用于管理请求超时
    DaneJoe::TimerManager& m_timer_manager;
    /// @brief 传输关联映射表，按请求ID组织传输关联
    std::unordered_map<uint64_t, TransCorrelation> m_trans_correlations;
    /// @brief 消息编解码器，负责消息的序列化和反序列化
    ClientMessageCodec m_message_codec;
    /// @brief 请求ID计数器，用于生成唯一的请求ID
    std::atomic<uint64_t> m_request_id_counter = 0;
    /// @brief 网络线程指针，网络服务运行在此线程中
    QThread* m_network_thread = nullptr;
    /// @brief 网络服务指针，负责底层网络通信
    NetworkService* m_network_service = nullptr;
};