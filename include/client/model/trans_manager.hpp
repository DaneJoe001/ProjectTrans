#pragma once

#include <unordered_map>

#include "client/model/file_info.hpp"

struct ProgressInfo
{
    FileInfo info;
    std::size_t currnt_count;
    std::size_t total_count;
};

class TransManager {
public:
    TransManager() = default;
    ~TransManager() = default;
    void add_trans(const FileInfo& info);
private:
    std::unordered_map<int, ProgressInfo> m_trans_map;
};