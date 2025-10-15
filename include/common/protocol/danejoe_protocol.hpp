#pragma once

#include <vector>
#include <string>
#include <cstdint>

#include "common/network/url_resolver.hpp"

namespace DaneJoe
{
    namespace Protocol
    {
        /**
         * @enum RequestType
         * @brief 请求类型
         */
        enum class RequestType
        {
            Unknown,
            /// @brief get请求
            Get,
            /// @brief post请求
            Post,
            /// @brief put请求
            Put,
            /// @brief delete请求
            Delete,
            /// @brief head请求
            Head,
            /// @brief options请求
            Options,
            /// @brief patch请求
            Patch
        };
        /**
         * @enum ResponseStatus
         * @brief 响应状态
         */
        enum class ResponseStatus:uint16_t
        {
            ///< 未知状态
            Unknown = 0,          
            /// @brief 请求成功
            Ok = 200,             
            /// @brief 请求成功并创建了资源
            Created = 201,        
            /// @brief 请求已接受，但未处理完成
            Accepted = 202,       
            /// @brief 请求成功，但没有内容返回
            NoContent = 204,      
            /// @brief 错误的请求
            BadRequest = 400,     
            /// @brief 未授权
            Unauthorized = 401,   
            /// @brief 禁止访问
            Forbidden = 403,      
            /// @brief 找不到资源
            NotFound = 404,       
            /// @brief 方法不被允许
            MethodNotAllowed = 405,
            /// @brief 请求冲突
            Conflict = 409,       
            /// @brief 服务器内部错误
            InternalServerError = 500, 
            /// @brief 未实现的功能
            NotImplemented = 501,  
            /// @brief 错误的网关
            BadGateway = 502,      
            /// @brief 服务不可用
            ServiceUnavailable = 503 
        };
        enum class ContentType:uint8_t
        {
            /// @brief 未知类型
            Unknown,
            /// @brief application/json
            Json,
            /// @brief danejoe serialized
            DaneJoe,
        };

        /**
         * @struct RequestInfo
         * @brief 客户端请求信息
         */
        struct RequestInfo
        {
          /// @brief 请求类型
          RequestType type;
          /// @brief Url信息
          UrlResolver::UrlInfo url_info;
          /// @brief 序列化的消息体
          std::vector<uint8_t> body;
        };
        struct ResponseInfo
        {
            /// @brief 响应状态
            ResponseStatus status;
            /// @brief 内容类型
            ContentType content_type;
            /// @brief 响应消息体
          std::vector<uint8_t> body;
        };
    }
    std::string to_string(Protocol::RequestType type);
    Protocol::RequestType to_request_type(const std::string& type_str);
    std::string to_string(Protocol::ResponseStatus status);
    Protocol::ResponseStatus to_response_status(const std::string& status_str);
    Protocol::ContentType to_content_type(uint8_t type_code);
    uint8_t to_code(Protocol::ContentType type);
    Protocol::ResponseStatus to_response_status(uint16_t status_code);
    uint16_t to_code(Protocol::ResponseStatus status);
    std::string to_string(Protocol::ContentType type);
}