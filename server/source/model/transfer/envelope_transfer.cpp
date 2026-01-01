#include <format>

#include "model/transfer/envelope_transfer.hpp"

#define ENUM_STR(enum) #enum
std::string to_string(ContentType content_type)
{
    switch (content_type)
    {
    case ContentType::Json:
        return ENUM_STR(ContentType::Json);
    case ContentType::DaneJoe:
        return ENUM_STR(ContentType::DaneJoe);
    default:
        return ENUM_STR(ContentType::Unknown);
    }

}

std::string to_string(ResponseStatus status)
{
    switch (status)
    {
    case ResponseStatus::Ok:
        return ENUM_STR(ResponseStatus::Ok);
    case ResponseStatus::Created:
        return ENUM_STR(ResponseStatus::Created);
    case ResponseStatus::Accepted:
        return ENUM_STR(ResponseStatus::Accepted);
    case ResponseStatus::NoContent:
        return ENUM_STR(ResponseStatus::NoContent);
    case ResponseStatus::BadRequest:
        return ENUM_STR(ResponseStatus::BadRequest);
    case ResponseStatus::Unauthorized:
        return ENUM_STR(ResponseStatus::Unauthorized);
    case ResponseStatus::Forbidden:
        return ENUM_STR(ResponseStatus::Forbidden);
    case ResponseStatus::NotFound:
        return ENUM_STR(ResponseStatus::NotFound);
    case ResponseStatus::MethodNotAllowed:
        return ENUM_STR(ResponseStatus::MethodNotAllowed);
    case ResponseStatus::Conflict:
        return ENUM_STR(ResponseStatus::Conflict);
    case ResponseStatus::InternalServerError:
        return ENUM_STR(ResponseStatus::InternalServerError);
    case ResponseStatus::NotImplemented:
        return ENUM_STR(ResponseStatus::NotImplemented);
    case ResponseStatus::BadGateway:
        return ENUM_STR(ResponseStatus::BadGateway);
    case ResponseStatus::ServiceUnavailable:
        return ENUM_STR(ResponseStatus::ServiceUnavailable);
    default:
        return ENUM_STR(ResponseStatus::Unknown);
    }

}

std::string EnvelopeRequestTransfer::to_string() const
{
    return std::format("body_size={} | content_type={} | path={} | request_id={} | request_type={} | version={}",
        body.size(), ::to_string(content_type), path, request_id, request_type, version);
}

std::string EnvelopeResponseTransfer::to_string() const
{
    return std::format("body_size={} | content_type={} | request_id={} | status={} | version={}",
        body.size(), ::to_string(content_type), request_id, ::to_string(status), version);
}