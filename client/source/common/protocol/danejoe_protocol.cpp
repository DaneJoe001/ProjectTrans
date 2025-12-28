#include "common/protocol/danejoe_protocol.hpp"

std::string DaneJoe::to_string(DaneJoe::Protocol::RequestType type)
{
    switch (type)
    {
    case Protocol::RequestType::Get:
        return "get";
    case Protocol::RequestType::Post:
        return "post";
    case Protocol::RequestType::Put:
        return "put";
    case Protocol::RequestType::Delete:
        return "delete";
    case Protocol::RequestType::Head:
        return "head";
    case Protocol::RequestType::Options:
        return "options";
    case Protocol::RequestType::Patch:
        return "patch";
    default:
        return "unknown";
    }
}

DaneJoe::Protocol::RequestType DaneJoe::to_request_type(const std::string& type_str)
{
if(type_str == "get")
    {
        return Protocol::RequestType::Get;
    }
    else if(type_str == "post")
    {
        return Protocol::RequestType::Post;
    }
    else if(type_str == "put")
    {
        return Protocol::RequestType::Put;
    }
    else if(type_str == "delete")
    {
        return Protocol::RequestType::Delete;
    }
    else if(type_str == "head")
    {
        return Protocol::RequestType::Head;
    }
    else if(type_str == "options")
    {
        return Protocol::RequestType::Options;
    }
    else if(type_str == "patch")
    {
        return Protocol::RequestType::Patch;
    }
    else
    {
        return Protocol::RequestType::Unknown;
    }
}

std::string DaneJoe::to_string(Protocol::ResponseStatus status)
{
    switch (status)
    {
    case Protocol::ResponseStatus::Ok:
        return "200 OK";
    case Protocol::ResponseStatus::Created:
        return "201 Created";
    case Protocol::ResponseStatus::Accepted:
        return "202 Accepted";
    case Protocol::ResponseStatus::NoContent:
        return "204 No Content";
    case Protocol::ResponseStatus::BadRequest:
        return "400 Bad Request";
    case Protocol::ResponseStatus::Unauthorized:
        return "401 Unauthorized";
    case Protocol::ResponseStatus::Forbidden:
        return "403 Forbidden";
    case Protocol::ResponseStatus::NotFound:
        return "404 Not Found";
    case Protocol::ResponseStatus::MethodNotAllowed:
        return "405 Method Not Allowed";
    case Protocol::ResponseStatus::Conflict:
        return "409 Conflict";
    case Protocol::ResponseStatus::InternalServerError:
        return "500 Internal Server Error";
    case Protocol::ResponseStatus::NotImplemented:
        return "501 Not Implemented";
    case Protocol::ResponseStatus::BadGateway:
        return "502 Bad Gateway";
    default:
        return "0 Unknown";
    }
}

DaneJoe::Protocol::ResponseStatus DaneJoe::to_response_status(const std::string& status_str)
{
    if (status_str == "200 OK")
    {
        return Protocol::ResponseStatus::Ok;
    }
    else if (status_str == "201 Created")
    {
        return Protocol::ResponseStatus::Created;
    }
    else if (status_str == "202 Accepted")
    {
        return Protocol::ResponseStatus::Accepted;
    }
    else if (status_str == "204 No Content")
    {
        return Protocol::ResponseStatus::NoContent;
    }
    else if (status_str == "400 Bad Request")
    {
        return Protocol::ResponseStatus::BadRequest;
    }
    else if (status_str == "401 Unauthorized")
    {
        return Protocol::ResponseStatus::Unauthorized;
    }
    else if (status_str == "403 Forbidden")
    {
        return Protocol::ResponseStatus::Forbidden;
    }
    else if (status_str == "404 Not Found")
    {
        return Protocol::ResponseStatus::NotFound;
    }
    else if (status_str == "405 Method Not Allowed")
    {
        return Protocol::ResponseStatus::MethodNotAllowed;
    }
    else if (status_str == "409 Conflict")
    {
        return Protocol::ResponseStatus::Conflict;
    }
    else if (status_str == "500 Internal Server Error")
    {
        return Protocol::ResponseStatus::InternalServerError;
    }
    else if (status_str == "501 Not Implemented")
    {
        return Protocol::ResponseStatus::NotImplemented;
    }
    else if (status_str == "502 Bad Gateway")
    {
        return Protocol::ResponseStatus::BadGateway;
    }
    else
    {
        return Protocol::ResponseStatus::Unknown;
    }
}

DaneJoe::Protocol::ContentType DaneJoe::to_content_type(uint8_t type_code)
{
    return static_cast<Protocol::ContentType>(type_code);
}

uint8_t DaneJoe::to_code(Protocol::ContentType type)
{
    return static_cast<uint16_t>(type);
}

DaneJoe::Protocol::ResponseStatus DaneJoe::to_response_status(uint16_t status_code)
{
    return static_cast<Protocol::ResponseStatus>(status_code);
}

uint16_t DaneJoe::to_code(Protocol::ResponseStatus status)
{
    return static_cast<uint16_t>(status);
}

std::string DaneJoe::to_string(Protocol::ContentType type)
{
    switch (type)
    {
    case Protocol::ContentType::DaneJoe:
        return "danejoe";
    case Protocol::ContentType::Json:
        return "json";
    default:
        return "unknown";
    }
}