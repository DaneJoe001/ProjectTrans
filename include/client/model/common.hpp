#pragma once

enum class FileState
{
    InTransfer,
    Completed,
    Failed
};

enum class Operation
{
    Download,
    Upload
};