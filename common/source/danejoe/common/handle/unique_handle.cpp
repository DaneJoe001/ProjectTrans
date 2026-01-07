#include "danejoe/common/status/posix_status_detail.hpp"
#include "danejoe/common/handle/unique_handle.hpp"

#if DANEJOE_PLATFORM_LINUX==1
#include <unistd.h>
#endif

DaneJoe::StatusCode DaneJoe::close(int& handle)
{
#if DANEJOE_PLATFORM_LINUX==1
    if (handle < 0)
    {
        return StatusCode(std::make_shared<PosixStatusDetail>(true));
    }
    int error_code = ::close(handle);
    handle = -1;
    if (error_code >= 0)
    {
        return StatusCode(std::make_shared<PosixStatusDetail>(true));
    }
    return StatusCode(std::make_shared<PosixStatusDetail>(false));
#endif
    return StatusCode(std::make_shared<PosixStatusDetail>(true));
}