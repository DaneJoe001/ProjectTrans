#include <chrono>
#include <thread>

#include "danejoe/common/diagnostic/diagnostic_system.hpp"

DaneJoe::DiagnosticSystem& DaneJoe::DiagnosticSystem::get_instance()
{
    static DiagnosticSystem instance;
    return instance;
}
DaneJoe::DiagnosticSystem::DiagnosticSystem() {}

std::string DaneJoe::format_message(std::string_view message)
{
    return std::string(message);
}
DaneJoe::DiagnosticSystem::~DiagnosticSystem() = default;
void DaneJoe::DiagnosticSystem::add_event(DiagnosticEventLevel level,
    const std::string& module_name,
    const std::string& message,
    int64_t line_number,
    const std::string& function_name,
    const std::string& file_name)
{
    DiagnosticEvent event;
    event.event_id = m_event_id++;
    event.level = level;
    event.module_name = module_name;
    event.message = message;
    event.line_number = line_number;
    event.function_name = function_name;
    event.occurrence_time = std::chrono::system_clock::now();
    event.thread_id = m_thread_id_hash(std::this_thread::get_id());
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_events.push_back(event);
    }
}
void DaneJoe::DiagnosticSystem::add_event(const DiagnosticEvent& event)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    m_events.push_back(event);
}
void DaneJoe::DiagnosticSystem::clear_events()
{
    std::lock_guard<std::mutex> lock(m_mutex);
    m_events.clear();
}
std::vector<DaneJoe::DiagnosticEvent> DaneJoe::DiagnosticSystem::get_events()
{
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_events;
}
std::vector<DaneJoe::DiagnosticEvent>
DaneJoe::DiagnosticSystem::get_events_by_module(const std::string& module_name)
{
    std::vector<DaneJoe::DiagnosticEvent> result;
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        for (auto& event : m_events)
        {
            if (event.module_name == module_name)
            {
                result.push_back(event);
            }
        }
    }
    return result;
}
