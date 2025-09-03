#pragma once
#include <cstdint>
#include <vector>
#include <memory>

class ISocket
{
public:
    struct IOption
    {
        int level;
        int opt_name;
        void* opt_val = nullptr;
        int opt_len;
    };
public:
    virtual ~ISocket() = default;
    virtual bool is_valid()const = 0;
    virtual bool set_opt(const IOption& option) = 0;
    virtual void close() = 0;
protected:
};