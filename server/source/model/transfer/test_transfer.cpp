#include <format>

#include "model/transfer/test_transfer.hpp"

std::string TestRequestTransfer::to_string() const
{
    return std::format("message={}", message);
}

std::string TestResponseTransfer::to_string() const
{
    return std::format("message={}", message);
}