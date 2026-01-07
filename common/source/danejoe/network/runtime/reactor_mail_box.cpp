#include "danejoe/network/runtime/reactor_mail_box.hpp"

DaneJoe::ReactorMailBox::ReactorMailBox()
{
    m_to_server_frame_queue = MpmcBoundedQueue<PosixFrame>(128);
}

DaneJoe::ReactorMailBox::~ReactorMailBox()
{
    stop();
}

void DaneJoe::ReactorMailBox::set_event_handle(std::shared_ptr<PosixEventHandle> event_handle)
{
    m_event_handle = event_handle;
}

void DaneJoe::ReactorMailBox::add_to_client_queue(uint64_t connect_id)
{
    std::lock_guard<std::mutex> lock(m_client_queues_mutex);
    m_to_client_queues[connect_id] = std::queue<PosixFrame>();
}

void DaneJoe::ReactorMailBox::remove_to_client_queue(uint64_t connect_id)
{
    std::lock_guard<std::mutex> lock(m_client_queues_mutex);
    auto it = m_to_client_queues.find(connect_id);
    if (it == m_to_client_queues.end())
    {
        return;
    }
    m_to_client_queues.erase(it);
}

void DaneJoe::ReactorMailBox::push_to_client_frame(
    const PosixFrame& frame)
{
    {
        std::lock_guard<std::mutex> lock(m_client_queues_mutex);
        auto it = m_to_client_queues.find(frame.connect_id);
        if (it == m_to_client_queues.end())
        {
            return;
        }
        it->second.push(frame);
    }
    if (m_event_handle)
    {
        m_event_handle->write(1);
    }
}
void DaneJoe::ReactorMailBox::push_to_server_frame(const PosixFrame& frame)
{
    m_to_server_frame_queue.push(frame);
}
void DaneJoe::ReactorMailBox::push_to_server_frame(const std::vector<PosixFrame>& frames)
{
    for (const auto& frame : frames)
    {
        m_to_server_frame_queue.push(frame);
    }
}
std::optional<DaneJoe::PosixFrame>  DaneJoe::ReactorMailBox::pop_from_to_client_queue(uint64_t connect_id)
{
    std::lock_guard<std::mutex> lock(m_client_queues_mutex);
    auto it = m_to_client_queues.find(connect_id);
    if (it == m_to_client_queues.end())
    {
        return std::nullopt;
    }
    if (it->second.empty())
    {
        return std::nullopt;
    }
    auto frame = it->second.front();
    it->second.pop();
    return frame;
}
std::optional<DaneJoe::PosixFrame>  DaneJoe::ReactorMailBox::pop_from_to_server_frame()
{
    return m_to_server_frame_queue.pop();
}

std::optional<DaneJoe::PosixFrame> DaneJoe::ReactorMailBox::try_pop_from_to_server_queue()
{
    return m_to_server_frame_queue.try_pop();
}
void DaneJoe::ReactorMailBox::stop()
{
    m_to_server_frame_queue.close();
}