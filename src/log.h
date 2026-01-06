#pragma once
#include <string>

namespace Log {

    enum class Level {
        Session,
        Debug,
        Error
    };

    void init();
    void shutdown();
    void write(Level lvl, const char* file, int line, const std::string& msg);

    void logGLError(const char* call, const char* file, int line);
}

// ================= CONFIG =================
#define ENABLE_SESSION_LOG
#define ENABLE_DEBUG_LOG
#define ENABLE_ERROR_LOG
#define ENABLE_GL_TRACE
#define ENABLE_CRASH_HANDLER
// ==========================================

#ifdef ENABLE_SESSION_LOG
#define LOG_SESSION(msg) Log::write(Log::Level::Session, __FILE__, __LINE__, msg)
#else
#define LOG_SESSION(msg) ((void)0)
#endif

#ifdef ENABLE_DEBUG_LOG
#define LOG_DEBUG(msg) Log::write(Log::Level::Debug, __FILE__, __LINE__, msg)
#else
#define LOG_DEBUG(msg) ((void)0)
#endif

#ifdef ENABLE_ERROR_LOG
#define LOG_ERROR(msg) Log::write(Log::Level::Error, __FILE__, __LINE__, msg)
#else
#define LOG_ERROR(msg) ((void)0)
#endif

#ifdef ENABLE_GL_TRACE
#define GL_CALL(x) do { x; Log::logGLError(#x, __FILE__, __LINE__); } while(0)
#else
#define GL_CALL(x) x
#endif
