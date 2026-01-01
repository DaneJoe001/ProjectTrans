#pragma once

#include <optional>

#include "model/transfer/envelope_transfer.hpp"
#include "model/transfer/block_transfer.hpp"
#include "model/transfer/download_transfer.hpp"
#include "model/transfer/test_transfer.hpp"

class ServerMessageCodec
{
public:
    std::optional<EnvelopeRequestTransfer> try_parse_byte_array_request(const std::vector<uint8_t>& data);
    std::optional<BlockRequestTransfer> try_parse_byte_array_block_request(const std::vector<uint8_t>& data);
    std::optional<DownloadRequestTransfer> try_parse_byte_array_download_request(const std::vector<uint8_t>& data);
    std::optional<TestRequestTransfer> try_parse_byte_array_test_request(const std::vector<uint8_t>& data);
    std::vector<uint8_t> build_response_byte_array(const EnvelopeResponseTransfer& response);
    std::vector<uint8_t> build_block_response_byte_array(const BlockResponseTransfer& block_response, int64_t request_id);
    std::vector<uint8_t> build_download_response_byte_array(const DownloadResponseTransfer& download_response, int64_t request_id);
    std::vector<uint8_t> build_test_response_byte_array(const TestResponseTransfer& block_response, int64_t request_id);
private:

};