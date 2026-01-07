#include "danejoe/common/system/system_info.hpp"

#if defined(_WIN32)
#include <windows.h>
#else
#include <unistd.h>
#endif


int DaneJoe::get_process_id()
{
#if defined(_WIN32)
    return static_cast<int>(::GetCurrentProcessId());
#else
    return static_cast<int>(::getpid());
#endif
}
