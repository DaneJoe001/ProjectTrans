#include <format>
#include "danejoe/stringify/stringify_to_string.hpp"
#include "danejoe/network/url/url_info.hpp"

std::string DaneJoe::to_string(UrlProtocol protocol)
{
    switch (protocol)
    {
    case UrlProtocol::Unknown:
        return ENUM_TO_STRING(UrlProtocol::Unknown);
    case UrlProtocol::Http:
        return ENUM_TO_STRING(UrlProtocol::Http);
    case UrlProtocol::Https:
        return ENUM_TO_STRING(UrlProtocol::Https);
    case UrlProtocol::Ftp:
        return ENUM_TO_STRING(UrlProtocol::Ftp);
    case UrlProtocol::Danejoe:
        return ENUM_TO_STRING(UrlProtocol::Danejoe);
    default:
        return ENUM_TO_STRING(UrlProtocol::Unknown);
    }
}

template<>
DaneJoe::UrlProtocol DaneJoe::enum_cast<DaneJoe::UrlProtocol>(const std::string& enum_string)
{
    if (enum_string == ENUM_TO_STRING(UrlProtocol::Unknown))
    {
        return UrlProtocol::Unknown;
    }
    else if (enum_string == ENUM_TO_STRING(UrlProtocol::Http))
    {
        return UrlProtocol::Http;
    }
    else if (enum_string == ENUM_TO_STRING(UrlProtocol::Https))
    {
        return UrlProtocol::Https;
    }
    else if (enum_string == ENUM_TO_STRING(UrlProtocol::Ftp))
    {
        return UrlProtocol::Ftp;
    }
    else if (enum_string == ENUM_TO_STRING(UrlProtocol::Danejoe))
    {
        return UrlProtocol::Danejoe;
    }
    return UrlProtocol::Unknown;
}

std::string DaneJoe::UrlInfo::to_string() const
{
    return std::format(
        "protocol={} | host={} | port={} | path={} | query={}",
        DaneJoe::to_string(protocol),
        host,
        port,
        path,
        DaneJoe::to_string(query));
}
