#include "common/network/i_socket_context.hpp"
#include "common/log/manage_logger.hpp"

void ISocketContext::set_recv_buffer(std::shared_ptr<DaneJoe::MTQueue<uint8_t>> buffer)
{
    if (!buffer)
    {
        DANEJOE_LOG_WARN("default", "ISocketContext", "buffer is null");
    }
    m_recv_buffer = buffer;
}
void ISocketContext::set_send_buffer(std::shared_ptr<DaneJoe::MTQueue<uint8_t>> buffer)
{
    if (!buffer)
    {
        DANEJOE_LOG_WARN("default", "ISocketContext", "buffer is null");
    }
    m_send_buffer = buffer;
}