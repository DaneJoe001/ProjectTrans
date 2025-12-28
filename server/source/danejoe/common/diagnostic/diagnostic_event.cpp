#include <format>

#include "danejoe/common/diagnostic/diagnostic_event.hpp"

std::string DaneJoe::to_string(DiagnosticEventLevel level)
{
    switch (level)
    {
    case DiagnosticEventLevel::Trace:
        return ENUM_TO_STRING(DiagnosticEventLevel::Trace);
    case DiagnosticEventLevel::Debug:
        return ENUM_TO_STRING(DiagnosticEventLevel::Debug);
    case DiagnosticEventLevel::Info:
        return ENUM_TO_STRING(DiagnosticEventLevel::Info);
    case DiagnosticEventLevel::Warn:
        return ENUM_TO_STRING(DiagnosticEventLevel::Warn);
    case DiagnosticEventLevel::Error:
        return ENUM_TO_STRING(DiagnosticEventLevel::Error);
    case DiagnosticEventLevel::Critical:
        return ENUM_TO_STRING(DiagnosticEventLevel::Critical);
    default:
        return ENUM_TO_STRING(DiagnosticEventLevel::Unknown);
    }
}
std::string DaneJoe::DiagnosticEvent::to_string() const
{
    return std::format(
        "occurrence_time={:%Y-%m-%d %H:%M:%S} | event_id={} | thread_id={} | level={} | module_name={} | "
        "function_name={} | line_number={} | file_name={} | message={}",
        occurrence_time,
        event_id,
        thread_id,
        DaneJoe::to_string(level),
        module_name,
        function_name,
        line_number,
        file_name,
        message);
}

template <>
DaneJoe::DiagnosticEventLevel DaneJoe::enum_cast<DaneJoe::DiagnosticEventLevel>(const std::string& enum_string)
{
    if (enum_string == ENUM_TO_STRING(DiagnosticEventLevel::Trace))
    {
        return DiagnosticEventLevel::Trace;
    }
    else if (enum_string == ENUM_TO_STRING(DiagnosticEventLevel::Debug))
    {
        return DiagnosticEventLevel::Debug;
    }
    else if (enum_string == ENUM_TO_STRING(DiagnosticEventLevel::Info))
    {
        return DiagnosticEventLevel::Info;
    }
    else if (enum_string == ENUM_TO_STRING(DiagnosticEventLevel::Warn))
    {
        return DiagnosticEventLevel::Warn;
    }
    else if (enum_string == ENUM_TO_STRING(DiagnosticEventLevel::Error))
    {
        return DiagnosticEventLevel::Error;
    }
    else if (enum_string == ENUM_TO_STRING(DiagnosticEventLevel::Critical))
    {
        return DiagnosticEventLevel::Critical;
    }
    return DiagnosticEventLevel::Unknown;
}
