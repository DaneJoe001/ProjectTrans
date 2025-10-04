#include "client/model/common.hpp"

std::string to_string(FileState state)
{
    switch (state)
    {
    case FileState::Completed:return "Completed";
    case FileState::Failed:return "Failed";
    case FileState::InTransfer:return "InTransfer";
    case FileState::Waiting:return "Waiting";
    default:return "Unknown";
    }
}

std::string to_string(Operation operation)
{
    switch (operation)
    {
    case Operation::Download:return "Download";
    case Operation::Upload:return "Upload";
    default:return "Unknown";
    }
}