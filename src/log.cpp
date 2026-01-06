#include "Log.h"
#include <windows.h>
#include <GL/gl.h>
#include <fstream>
#include <filesystem>
#include <mutex>
#include <chrono>
#include <sstream>
#include <iomanip>

namespace {
    std::ofstream sessionFile, debugFile, errorFile;
    std::mutex logMutex;

    std::string timestamp() {
        auto now = std::chrono::system_clock::now();
        auto t = std::chrono::system_clock::to_time_t(now);
        std::tm tm{};
        localtime_s(&tm, &t);
        std::ostringstream ss;
        ss << std::put_time(&tm, "%Y-%m-%d %H:%M:%S");
        return ss.str();
    }

    const char* glErr(GLenum e) {
        switch (e) {
            case GL_INVALID_ENUM: return "GL_INVALID_ENUM";
            case GL_INVALID_VALUE: return "GL_INVALID_VALUE";
            case GL_INVALID_OPERATION: return "GL_INVALID_OPERATION";
            case GL_OUT_OF_MEMORY: return "GL_OUT_OF_MEMORY";
            default: return "UNKNOWN_GL_ERROR";
        }
    }
}

namespace Log {

    void init() {
        std::filesystem::create_directories("log");
        sessionFile.open("log/session.log", std::ios::trunc);
        debugFile.open("log/debug.log", std::ios::trunc);
        errorFile.open("log/err.log", std::ios::trunc);
        LOG_DEBUG("Logger initialized");
    }

    void shutdown() {
        LOG_DEBUG("Logger shutdown");
        if (sessionFile.is_open()) sessionFile.close();
        if (debugFile.is_open()) debugFile.close();
        if (errorFile.is_open()) errorFile.close();
    }

    void write(Level lvl, const char* file, int line, const std::string& msg) {
        std::lock_guard<std::mutex> lock(logMutex);
        std::ostringstream out;
        out << "[" << timestamp() << "] "
            << file << ":" << line << " | "
            << msg << "\n";

        if (lvl == Level::Session && sessionFile.is_open()) sessionFile << out.str();
        if (lvl == Level::Debug && debugFile.is_open()) debugFile << out.str();
        if (lvl == Level::Error && errorFile.is_open()) errorFile << out.str();
    }

    void logGLError(const char* call, const char* file, int line) {
        GLenum err;
        while ((err = glGetError()) != GL_NO_ERROR) {
            write(Level::Error, file, line,
                std::string("OpenGL error after ") + call + " -> " + glErr(err));
        }
    }
}
