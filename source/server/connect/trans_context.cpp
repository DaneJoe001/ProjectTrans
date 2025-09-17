#include <vector>
#include <string>
#include <cstring>
#include <thread>
#include <queue>

#include "server/connect/trans_context.hpp"
#include "log/manage_logger.hpp"
#include "mt_queue/mt_queue.hpp"

DaneJoe::MTQueue<uint8_t> temp(4096);

#define ADD_LENGTH_INFO_TO_RECV
#define ADD_LENGTH_INFO_TO_SEND

void TransContext::on_recv()
{
#ifdef ADD_LENGTH_INFO_TO_RECV
    auto length_data_optional = m_recv_buffer->pop(sizeof(uint32_t));
    std::vector<uint8_t> length_data;
    if (length_data_optional.has_value())
    {
        length_data = length_data_optional.value();
    }
    else
    {
        return;
    }
    if (length_data.empty() || length_data.size() != sizeof(uint32_t))
    {
        return;
    }
    uint32_t size = 0;
    std::memcpy(&size, length_data.data(), sizeof(uint32_t));
    auto data_optional = m_recv_buffer->pop(size);
    std::vector<uint8_t> data;
    if (data_optional.has_value())
    {
        data = data_optional.value();
    }
#else
    auto data = buffer->try_pop(1024);
#endif

    std::string str(data.begin(), data.end());
    DANEJOE_LOG_INFO("default", "TransContext", "TransContext recieved: {}", str);
    for (auto ch : str)
    {
        temp.push((uint8_t)ch);
    }
}

void TransContext::on_send()
{
    DANEJOE_LOG_TRACE("default", "TransContext", "TransContext send");
#ifdef ADD_LENGTH_INFO_TO_SEND
    uint32_t size = temp.size();
    std::vector<uint8_t> length_data(sizeof(uint32_t));
    std::memcpy(length_data.data(), &size, sizeof(uint32_t));
    m_send_buffer->push(length_data.begin(), length_data.end());

    auto data_optional = temp.pop(size);
    std::vector<uint8_t> data;
    if (data_optional.has_value())
    {
        data = data_optional.value();
    }
#else
    auto data = temp.try_pop(1024);
#endif
    m_send_buffer->push(data.begin(), data.end());
}
std::shared_ptr<ISocketContext> TransContextCreator::create()
{
    return std::make_shared<TransContext>();
}

std::shared_ptr<ISocketContext> TransContextCreator::create(
    std::shared_ptr<DaneJoe::MTQueue<uint8_t>> recv_buffer,
    std::shared_ptr<DaneJoe::MTQueue<uint8_t>> send_buffer)
{
    auto context = std::make_shared<TransContext>();
    context->set_recv_buffer(recv_buffer);
    context->set_send_buffer(send_buffer);
    return context;
}
