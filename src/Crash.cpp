#include "Crash.h"
#include "Log.h"
#include <windows.h>
#include <sstream>

static LONG WINAPI CrashFilter(EXCEPTION_POINTERS* ep) {
    std::ostringstream ss;
    ss << "CRASH\n"
       << "Exception: 0x" << std::hex << ep->ExceptionRecord->ExceptionCode
       << "\nAddress: " << ep->ExceptionRecord->ExceptionAddress;

    LOG_ERROR(ss.str());
    Log::shutdown();
    return EXCEPTION_EXECUTE_HANDLER;
}

void InstallCrashHandler() {
#ifdef ENABLE_CRASH_HANDLER
    SetUnhandledExceptionFilter(CrashFilter);
    LOG_DEBUG("Crash handler installed");
#endif
}
