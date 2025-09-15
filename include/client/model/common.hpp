#pragma once

enum class FileState
{
    Unknown,
    Waiting,
    InTransfer,
    Completed,
    Failed,
};

enum class Operation
{
    Unknown,
    Download,
    Upload
};