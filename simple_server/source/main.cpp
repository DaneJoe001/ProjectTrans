#include <cstdint>
#include <cstring>
#include <fstream>
#include <iostream>
#include <optional>
#include <string>
#include <thread>
#include <vector>

#include <algorithm>

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#include <filesystem>

#include "danejoe/network/codec/frame_assembler.hpp"
#include "danejoe/network/codec/serialize_codec.hpp"
#include "danejoe/network/codec/serialize_field.hpp"
#include "danejoe/network/codec/serialize_array_value.hpp"

#include "model/transfer/envelope_transfer.hpp"

namespace
{
constexpr uint16_t kDefaultPort = 8080;
constexpr int kBacklog = 128;

struct ServerConfig
{
    uint16_t port = kDefaultPort;
    std::string fixture_file_path;
};

ServerConfig g_config;

std::filesystem::path resource_file_dir()
{
#ifdef SIMPLE_SERVER_RESOURCE_DIR
    return std::filesystem::path(SIMPLE_SERVER_RESOURCE_DIR) / "file";
#else
    return std::filesystem::path();
#endif
}

std::vector<std::filesystem::path> list_resource_files_sorted()
{
    std::vector<std::filesystem::path> files;
    auto dir = resource_file_dir();
    if (dir.empty())
    {
        return files;
    }

    std::error_code ec;
    if (!std::filesystem::exists(dir, ec) || ec)
    {
        return files;
    }

    for (const auto& entry : std::filesystem::directory_iterator(dir, ec))
    {
        if (ec)
        {
            break;
        }
        std::error_code file_ec;
        if (entry.is_regular_file(file_ec) && !file_ec)
        {
            files.push_back(entry.path());
        }
    }

    std::sort(files.begin(), files.end(), [](const std::filesystem::path& a, const std::filesystem::path& b)
        {
            return a.filename().string() < b.filename().string();
        });
    return files;
}

std::optional<std::filesystem::path> resolve_file_path_for_file_id(int64_t file_id)
{
    if (file_id <= 0)
    {
        return std::nullopt;
    }

    static const std::vector<std::filesystem::path> kFiles = list_resource_files_sorted();
    const size_t idx = static_cast<size_t>(file_id - 1);
    if (idx >= kFiles.size())
    {
        return std::nullopt;
    }
    return kFiles[idx];
}

std::optional<int64_t> try_get_file_size(const std::string& path)
{
    std::error_code ec;
    auto sz = std::filesystem::file_size(path, ec);
    if (ec)
    {
        return std::nullopt;
    }
    return static_cast<int64_t>(sz);
}

std::string get_basename(const std::string& path)
{
    std::error_code ec;
    auto p = std::filesystem::path(path);
    auto name = p.filename().string();
    if (name.empty())
    {
        return std::string("fixture.bin");
    }
    return name;
}

std::optional<EnvelopeRequestTransfer> try_parse_envelope_request(const std::vector<uint8_t>& frame)
{
    DaneJoe::SerializeCodec serializer;
    serializer.deserialize(frame);

    auto version_field_opt = serializer.get_parsed_field("version");
    auto request_id_field_opt = serializer.get_parsed_field("request_id");
    auto request_type_field_opt = serializer.get_parsed_field("request_type");
    auto path_field_opt = serializer.get_parsed_field("path");
    auto content_type_field_opt = serializer.get_parsed_field("content_type");
    auto body_field_opt = serializer.get_parsed_field("body");

    if (!version_field_opt.has_value() || !request_id_field_opt.has_value() ||
        !request_type_field_opt.has_value() || !path_field_opt.has_value() ||
        !content_type_field_opt.has_value() || !body_field_opt.has_value())
    {
        return std::nullopt;
    }

    EnvelopeRequestTransfer req;

    auto version_opt = DaneJoe::to_value<uint16_t>(version_field_opt.value());
    auto request_id_opt = DaneJoe::to_value<uint64_t>(request_id_field_opt.value());
    auto request_type_opt = DaneJoe::to_value<uint8_t>(request_type_field_opt.value());
    auto content_type_opt = DaneJoe::to_value<uint8_t>(content_type_field_opt.value());

    if (!version_opt.has_value() || !request_id_opt.has_value() || !request_type_opt.has_value() || !content_type_opt.has_value())
    {
        return std::nullopt;
    }

    req.version = version_opt.value();
    req.request_id = request_id_opt.value();
    req.request_type = request_type_opt.value();
    req.path = DaneJoe::to_string(path_field_opt.value());
    req.content_type = static_cast<ContentType>(content_type_opt.value());
    req.body = DaneJoe::to_array<uint8_t>(body_field_opt.value());

    return req;
}

std::vector<uint8_t> build_envelope_response(const EnvelopeResponseTransfer& info)
{
    DaneJoe::SerializeCodec serializer;
    serializer.serialize(info.version, "version");
    serializer.serialize(info.request_id, "request_id");
    serializer.serialize(static_cast<uint16_t>(info.status), "status");
    serializer.serialize(static_cast<uint8_t>(info.content_type), "content_type");
    serializer.serialize(info.body, "body");
    return serializer.get_serialized_data_vector_build();
}

std::vector<uint8_t> build_test_response_body(const std::string& message)
{
    DaneJoe::SerializeCodec serializer;
    serializer.serialize(message, "message");
    return serializer.get_serialized_data_vector_build();
}

std::vector<uint8_t> build_download_response_body(
    int64_t task_id,
    int64_t file_id,
    const std::string& file_name,
    int64_t file_size,
    const std::string& md5_code)
{
    DaneJoe::SerializeCodec serializer;
    serializer.serialize(task_id, "task_id");
    serializer.serialize(file_id, "file_id");
    serializer.serialize(file_name, "file_name");
    serializer.serialize(file_size, "file_size");
    serializer.serialize(md5_code, "md5_code");
    return serializer.get_serialized_data_vector_build();
}

std::vector<uint8_t> build_block_response_body(
    int64_t block_id,
    int64_t file_id,
    int64_t task_id,
    int64_t offset,
    int64_t block_size,
    std::vector<uint8_t> data)
{
    DaneJoe::SerializeCodec serializer;
    serializer.serialize(block_id, "block_id");
    serializer.serialize(file_id, "file_id");
    serializer.serialize(task_id, "task_id");
    serializer.serialize(offset, "offset");
    serializer.serialize(block_size, "block_size");
    serializer.serialize(data, "data");
    return serializer.get_serialized_data_vector_build();
}

EnvelopeResponseTransfer dispatch_request(const EnvelopeRequestTransfer& req)
{
    EnvelopeResponseTransfer resp;
    resp.version = req.version;
    resp.request_id = req.request_id;
    resp.status = ResponseStatus::Ok;
    resp.content_type = ContentType::DaneJoe;

    if (req.path == "/test")
    {
        DaneJoe::SerializeCodec body_serializer;
        body_serializer.deserialize(req.body);
        auto msg_field_opt = body_serializer.get_parsed_field("message");
        std::string msg = msg_field_opt.has_value() ? DaneJoe::to_string(msg_field_opt.value()) : std::string("<empty>");
        resp.body = build_test_response_body(std::string("echo: ") + msg);
        return resp;
    }

    if (req.path == "/download")
    {
        int64_t task_id = -1;
        int64_t file_id = 1;
        {
            DaneJoe::SerializeCodec body_serializer;
            body_serializer.deserialize(req.body);

            auto task_id_field_opt = body_serializer.get_parsed_field("task_id");
            if (task_id_field_opt.has_value())
            {
                auto task_id_opt = DaneJoe::to_value<int64_t>(task_id_field_opt.value());
                if (task_id_opt.has_value())
                {
                    task_id = task_id_opt.value();
                }
            }

            auto file_id_field_opt = body_serializer.get_parsed_field("file_id");
            if (file_id_field_opt.has_value())
            {
                auto file_id_opt = DaneJoe::to_value<int64_t>(file_id_field_opt.value());
                if (file_id_opt.has_value())
                {
                    file_id = file_id_opt.value();
                }
            }
        }

        std::optional<std::filesystem::path> resolved_path;
        if (!g_config.fixture_file_path.empty())
        {
            resolved_path = std::filesystem::path(g_config.fixture_file_path);
        }
        else
        {
            resolved_path = resolve_file_path_for_file_id(file_id);
        }

        if (resolved_path.has_value())
        {
            const std::string path_str = resolved_path->string();
            auto file_size_opt = try_get_file_size(path_str);
            int64_t file_size = file_size_opt.has_value() ? file_size_opt.value() : 0;
            resp.body = build_download_response_body(
                task_id,
                file_id,
                resolved_path->filename().string(),
                file_size,
                "");
            return resp;
        }

        resp.status = ResponseStatus::NotFound;
        resp.body = build_test_response_body("file_id not found");
        return resp;
    }

    if (req.path == "/block")
    {
        int64_t block_id = 1;
        int64_t file_id = 1;
        int64_t task_id = -1;
        int64_t offset = 0;
        int64_t block_size = 1024;
        {
            DaneJoe::SerializeCodec body_serializer;
            body_serializer.deserialize(req.body);

            auto block_id_field_opt = body_serializer.get_parsed_field("block_id");
            if (block_id_field_opt.has_value())
            {
                auto block_id_opt = DaneJoe::to_value<int64_t>(block_id_field_opt.value());
                if (block_id_opt.has_value())
                {
                    block_id = block_id_opt.value();
                }
            }

            auto file_id_field_opt = body_serializer.get_parsed_field("file_id");
            if (file_id_field_opt.has_value())
            {
                auto file_id_opt = DaneJoe::to_value<int64_t>(file_id_field_opt.value());
                if (file_id_opt.has_value())
                {
                    file_id = file_id_opt.value();
                }
            }

            auto task_id_field_opt = body_serializer.get_parsed_field("task_id");
            if (task_id_field_opt.has_value())
            {
                auto task_id_opt = DaneJoe::to_value<int64_t>(task_id_field_opt.value());
                if (task_id_opt.has_value())
                {
                    task_id = task_id_opt.value();
                }
            }

            auto offset_field_opt = body_serializer.get_parsed_field("offset");
            if (offset_field_opt.has_value())
            {
                auto offset_opt = DaneJoe::to_value<int64_t>(offset_field_opt.value());
                if (offset_opt.has_value())
                {
                    offset = offset_opt.value();
                }
            }

            auto block_size_field_opt = body_serializer.get_parsed_field("block_size");
            if (block_size_field_opt.has_value())
            {
                auto block_size_opt = DaneJoe::to_value<int64_t>(block_size_field_opt.value());
                if (block_size_opt.has_value())
                {
                    block_size = block_size_opt.value();
                }
            }
        }

        if (block_size < 0)
        {
            block_size = 0;
        }

        std::vector<uint8_t> data;

        std::optional<std::filesystem::path> resolved_path;
        if (!g_config.fixture_file_path.empty())
        {
            resolved_path = std::filesystem::path(g_config.fixture_file_path);
        }
        else
        {
            resolved_path = resolve_file_path_for_file_id(file_id);
        }

        if (resolved_path.has_value())
        {
            const std::string path_str = resolved_path->string();
            auto file_size_opt = try_get_file_size(path_str);
            int64_t file_size = file_size_opt.has_value() ? file_size_opt.value() : 0;
            if (offset < 0)
            {
                offset = 0;
            }
            if (offset > file_size)
            {
                offset = file_size;
            }
            int64_t can_read = file_size - offset;
            if (can_read < 0)
            {
                can_read = 0;
            }
            if (block_size > can_read)
            {
                block_size = can_read;
            }

            data.resize(static_cast<size_t>(block_size));
            std::ifstream fin(path_str, std::ios::binary);
            if (fin)
            {
                fin.seekg(static_cast<std::streamoff>(offset), std::ios::beg);
                fin.read(reinterpret_cast<char*>(data.data()), static_cast<std::streamsize>(data.size()));
                auto read_bytes = static_cast<int64_t>(fin.gcount());
                if (read_bytes < 0)
                {
                    read_bytes = 0;
                }
                data.resize(static_cast<size_t>(read_bytes));
                block_size = read_bytes;
            }
            else
            {
                data.clear();
                block_size = 0;
            }
        }
        else
        {
            resp.status = ResponseStatus::NotFound;
            resp.body = build_test_response_body("file_id not found");
            return resp;
        }

        resp.body = build_block_response_body(block_id, file_id, task_id, offset, block_size, std::move(data));
        return resp;
    }

    resp.status = ResponseStatus::NotFound;
    resp.body = build_test_response_body("path not found");
    return resp;
}

bool send_all(int fd, const uint8_t* data, size_t size)
{
    size_t sent = 0;
    while (sent < size)
    {
        ssize_t n = ::send(fd, data + sent, size - sent, 0);
        if (n <= 0)
        {
            return false;
        }
        sent += static_cast<size_t>(n);
    }
    return true;
}

void handle_client(int client_fd)
{
    DaneJoe::FrameAssembler assembler;
    std::vector<uint8_t> buf(4096);

    while (true)
    {
        ssize_t n = ::recv(client_fd, buf.data(), buf.size(), 0);
        if (n == 0)
        {
            break;
        }
        if (n < 0)
        {
            break;
        }

        std::vector<uint8_t> chunk(buf.begin(), buf.begin() + n);
        assembler.push_data(chunk);

        while (true)
        {
            auto frame_opt = assembler.pop_frame();
            if (!frame_opt.has_value())
            {
                break;
            }

            auto req_opt = try_parse_envelope_request(frame_opt.value());
            EnvelopeResponseTransfer resp;
            if (!req_opt.has_value())
            {
                resp.version = 1;
                resp.request_id = 0;
                resp.status = ResponseStatus::BadRequest;
                resp.content_type = ContentType::DaneJoe;
                resp.body = build_test_response_body("bad request");
            }
            else
            {
                resp = dispatch_request(req_opt.value());
            }

            auto resp_frame = build_envelope_response(resp);
            if (!send_all(client_fd, resp_frame.data(), resp_frame.size()))
            {
                ::close(client_fd);
                return;
            }
        }
    }

    ::close(client_fd);
}

} // namespace

void print_usage(const char* argv0)
{
    std::cerr << "Usage: " << argv0 << " [--port <port>] [--fixture <file_path>]" << std::endl;
}

bool parse_args(int argc, char** argv)
{
    for (int i = 1; i < argc; ++i)
    {
        std::string arg = argv[i];
        if (arg == "--help" || arg == "-h")
        {
            return false;
        }
        if (arg == "--port" && i + 1 < argc)
        {
            auto port = std::stoi(argv[++i]);
            if (port <= 0 || port > 65535)
            {
                return false;
            }
            g_config.port = static_cast<uint16_t>(port);
            continue;
        }
        if (arg == "--fixture" && i + 1 < argc)
        {
            g_config.fixture_file_path = argv[++i];
            continue;
        }
        return false;
    }
    return true;
}

int main(int argc, char** argv)
{
    if (!parse_args(argc, argv))
    {
        print_usage(argv[0]);
        return 1;
    }

    int listen_fd = ::socket(AF_INET, SOCK_STREAM, 0);
    if (listen_fd < 0)
    {
        std::cerr << "socket() failed\n";
        return 1;
    }

    int opt = 1;
    ::setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    sockaddr_in addr;
    std::memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(g_config.port);
    addr.sin_addr.s_addr = INADDR_ANY;

    if (::bind(listen_fd, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) < 0)
    {
        std::cerr << "bind() failed\n";
        ::close(listen_fd);
        return 1;
    }

    if (::listen(listen_fd, kBacklog) < 0)
    {
        std::cerr << "listen() failed\n";
        ::close(listen_fd);
        return 1;
    }

    std::cerr << "simple_server listening on 0.0.0.0:" << g_config.port << "\n";
    if (!g_config.fixture_file_path.empty())
    {
        std::cerr << "fixture: " << g_config.fixture_file_path << "\n";
    }
    else
    {
        auto files = list_resource_files_sorted();
        std::cerr << "resource dir: " << resource_file_dir().string() << "\n";
        std::cerr << "resource files: " << files.size() << "\n";
    }

    while (true)
    {
        int client_fd = ::accept(listen_fd, nullptr, nullptr);
        if (client_fd < 0)
        {
            continue;
        }
        std::thread(&handle_client, client_fd).detach();
    }

    ::close(listen_fd);
    return 0;
}
