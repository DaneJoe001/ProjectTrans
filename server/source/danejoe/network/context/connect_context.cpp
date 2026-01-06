#include "danejoe/network/context/connect_context.hpp"
#include "danejoe/common/status/i_status_detail.hpp"
#include "danejoe/network/status/posix_status_code.hpp"
 #include "danejoe/common/diagnostic/diagnostic_system.hpp"

DaneJoe::ConnectContext::ConnectContext(
    uint64_t connect_id,
    PosixSocketHandle&& socket_handle) :
    m_connect_id(connect_id),
    m_socket_handle(std::move(socket_handle))
{

}
DaneJoe::Result<std::vector<DaneJoe::PosixFrame>> DaneJoe::ConnectContext::read()
{
    if (!m_socket_handle)
    {
        auto status_code = make_posix_status_code(StatusLevel::Error, "failed to read invalid socket");
        return Result<std::vector<PosixFrame>>(status_code);
    }
    int read_blocks = 0;
    while (true)
    {
        auto ret = m_socket_handle.read_some(1024);
        if (ret.status_code().get_status_level() == StatusLevel::Error)
        {
            ADD_DIAG_WARN("network", "ConnectContext::read error: connect_id={}, fd={}, status={}",
                m_connect_id,
                m_socket_handle.get_handle().get(),
                ret.status_code().message());
            return Result<std::vector<PosixFrame>>(ret.status_code());
        }
        if (!ret.has_value())
        {
            if (ret.status_code().get_status_level() == StatusLevel::Branch)
            {
                ADD_DIAG_DEBUG("network", "ConnectContext::read branch: connect_id={}, fd={}, status={}",
                    m_connect_id,
                    m_socket_handle.get_handle().get(),
                    ret.status_code().message());
            }
            break;
        }
        if (ret.value().empty())
        {
            ADD_DIAG_INFO("network", "ConnectContext::read peer closed: connect_id={}, fd={}",
                m_connect_id,
                m_socket_handle.get_handle().get());
            break;
        }
        read_blocks++;
        ADD_DIAG_DEBUG("network", "ConnectContext::read got bytes: connect_id={}, fd={}, size={}",
            m_connect_id,
            m_socket_handle.get_handle().get(),
            static_cast<int>(ret.value().size()));
        m_frame_assembler.push_data(ret.value());
    }
    std::vector<PosixFrame> result_frames;
    while (true)
    {
        auto frame_opt =
            m_frame_assembler.pop_frame();
        if (!frame_opt.has_value())
        {
            break;
        }
        ADD_DIAG_DEBUG("network", "ConnectContext::read pop frame: connect_id={}, size={}",
            m_connect_id,
            static_cast<int>(frame_opt.value().size()));
        result_frames.push_back({ m_connect_id,frame_opt.value() });
    }
    if (read_blocks > 0 || !result_frames.empty())
    {
        ADD_DIAG_DEBUG("network", "ConnectContext::read done: connect_id={}, fd={}, read_blocks={}, frames={}",
            m_connect_id,
            m_socket_handle.get_handle().get(),
            read_blocks,
            static_cast<int>(result_frames.size()));
    }
    auto status_code = make_posix_status_code(StatusLevel::Ok);
    return Result<std::vector<PosixFrame>>(result_frames, status_code);

}
DaneJoe::Result<int> DaneJoe::ConnectContext::write(std::vector<PosixFrame> frames)
{
    if (!m_socket_handle)
    {
        auto status_code = make_posix_status_code(StatusLevel::Error, "failed to read invalid socket");
        return Result<int>(status_code);
    }
    for (const auto& frame : frames)
    {
        m_write_buffer.insert(m_write_buffer.end(), frame.data.begin(), frame.data.end());
    }
    auto ret =
        m_socket_handle.write(m_write_buffer);
    if (ret.status_code().get_status_level() == StatusLevel::Error)
    {
        return Result<int>(ret.status_code());
    }
    if (!ret.has_value() || ret.value() == 0)
    {
        return Result<int>(ret.status_code());
    }
    int total_write = ret.value();
    m_write_buffer.erase(m_write_buffer.begin(), m_write_buffer.begin() + total_write);

    auto status_code = make_posix_status_code(StatusLevel::Ok);
    return Result<int>(total_write, status_code);
}

bool DaneJoe::ConnectContext::has_pending_write() const
{
    return !m_write_buffer.empty();
}

uint64_t DaneJoe::ConnectContext::get_connect_id()const
{
    return m_connect_id;
}