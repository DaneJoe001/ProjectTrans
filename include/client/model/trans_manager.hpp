#pragma once

#include <unordered_map>

#include "client/model/client_file_info.hpp"

struct ProgressInfo
{
    ClientFileInfo info;
    std::size_t current_count;
    std::size_t total_count;
};

class TransManager {
public:
    TransManager() = default;
    ~TransManager() = default;
    void add_trans(const ClientFileInfo& info);
private:
    std::unordered_map<int, ProgressInfo> m_trans_map;
};