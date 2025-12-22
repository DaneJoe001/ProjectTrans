#include "danejoe/network/context/i_socket_context.hpp"
#include "danejoe/common/diagnostic/diagnostic_system.hpp"

void DaneJoe::ISocketContext::set_recv_buffer(std::shared_ptr<DaneJoe::MpmcBoundedQueue<uint8_t>> buffer)
{
    if (!buffer)
    {
        ADD_DIAG_WARN("network", "Set recv buffer failed: buffer is null");
    }
    m_recv_buffer = buffer;
}
void DaneJoe::ISocketContext::set_send_buffer(std::shared_ptr<DaneJoe::MpmcBoundedQueue<uint8_t>> buffer)
{
    if (!buffer)
    {
        ADD_DIAG_WARN("network", "Set send buffer failed: buffer is null");
    }
    m_send_buffer = buffer;
}
