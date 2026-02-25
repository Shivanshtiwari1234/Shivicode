# Shivicode

Shivicode is a minimal Win32 + OpenGL text editor prototype written in C++20.

## Features

- Native Win32 window and message loop
- Immediate-mode OpenGL rendering
- Basic text editing:
  - character input
  - `Enter` for new lines
  - `Backspace` for deletion and line merge
- Blinking caret
- Crash handler hook (guarded by compile-time flag)
- Multi-file logging (`session`, `debug`, `error`)

## Project Structure

- `src/Shivicode.cpp`: app entry point, window + OpenGL setup, main loop
- `src/Editor.{h,cpp}`: text buffer, caret logic, rendering
- `src/log.{h,cpp}`: logging API and implementation
- `src/Crash.{h,cpp}`: unhandled exception filter setup
- `run.bat`: local build + run script

## Requirements

- Windows
- `g++` with C++20 support (MinGW-w64 recommended)
- OpenGL/Win32 system libs (already available on Windows)

## Build

```powershell
g++ src\*.cpp -lopengl32 -lgdi32 -luser32 -std=c++20 -Wall -Wextra -o Shivicode.exe
```

Or use:

```powershell
.\run.bat
```

## Run

```powershell
.\Shivicode.exe
```

## Test

Build and run the editor logic tester:

```powershell
g++ src\Editor.cpp src\EditorTester.cpp -lopengl32 -lgdi32 -luser32 -std=c++20 -Wall -Wextra -o EditorTester.exe
.\EditorTester.exe
```

## Controls

- Type printable keys to insert text
- `Enter`: split line / create new line
- `Backspace`: delete previous character, or merge with previous line at column 0
- Close window to exit

## Logging

At startup, logs are written to the `log/` directory:

- `log/session.log`
- `log/debug.log`
- `log/err.log`

Compile-time toggles are defined in `src/log.h`:

- `ENABLE_SESSION_LOG`
- `ENABLE_DEBUG_LOG`
- `ENABLE_ERROR_LOG`
- `ENABLE_GL_TRACE`
- `ENABLE_CRASH_HANDLER`
