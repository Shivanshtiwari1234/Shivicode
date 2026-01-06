#include <windows.h>
#include <GL/gl.h>
#include "Log.h"
#include "Crash.h"
#include "Editor.h"   // your existing editor

static HWND hwnd;
static HDC hdc;
static HGLRC hglrc;
static bool running = true;

static Editor editor;

// --------------------
// Window procedure
// --------------------
LRESULT CALLBACK WndProc(HWND h, UINT msg, WPARAM w, LPARAM l) {

    switch (msg) {

    case WM_CHAR:
        if (w == '\r') {
            editor.onEnter();
            LOG_SESSION("Enter");
        } else if (w >= 32) {
            editor.onChar((char)w);
            LOG_SESSION("Char input");
        }
        return 0;

    case WM_KEYDOWN:
        if (w == VK_BACK) {
            editor.onBackspace();
            LOG_SESSION("Backspace");
            return 0;
        }
        return 0;

    case WM_SIZE:
        editor.onResize(LOWORD(l), HIWORD(l));
        return 0;

    case WM_DESTROY:
        running = false;
        PostQuitMessage(0);
        return 0;
    }

    return DefWindowProc(h, msg, w, l);
}

// --------------------
// Entry point
// --------------------
int WINAPI WinMain(HINSTANCE hInst, HINSTANCE, LPSTR, int) {

    Log::init();
    InstallCrashHandler();
    LOG_DEBUG("Shivicode editor host starting");

    // Window class
    WNDCLASSA wc{};
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInst;
    wc.lpszClassName = "ShivicodeEditor";
    wc.style = CS_OWNDC;

    RegisterClassA(&wc);

    hwnd = CreateWindowExA(
        0,
        wc.lpszClassName,
        "Shivicode — Made in C++, Made for C++",
        WS_OVERLAPPEDWINDOW | WS_VISIBLE,
        100, 100, 1000, 700,
        nullptr, nullptr, hInst, nullptr
    );

    if (!hwnd) {
        LOG_ERROR("CreateWindowExA failed");
        return 0;
    }

    // OpenGL
    hdc = GetDC(hwnd);

    PIXELFORMATDESCRIPTOR pfd{};
    pfd.nSize = sizeof(pfd);
    pfd.nVersion = 1;
    pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
    pfd.iPixelType = PFD_TYPE_RGBA;
    pfd.cColorBits = 32;

    int pf = ChoosePixelFormat(hdc, &pfd);
    SetPixelFormat(hdc, pf, &pfd);

    hglrc = wglCreateContext(hdc);
    wglMakeCurrent(hdc, hglrc);

    LOG_DEBUG("OpenGL ready");

    // --------------------
    // Main editor loop
    // --------------------
    MSG msg;
    while (running) {

        while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        GL_CALL(glClearColor(0.06f, 0.06f, 0.07f, 1.0f));
        GL_CALL(glClear(GL_COLOR_BUFFER_BIT));

        editor.render();   // ← THIS is the key difference

        SwapBuffers(hdc);
    }

    // Cleanup
    wglMakeCurrent(nullptr, nullptr);
    wglDeleteContext(hglrc);
    ReleaseDC(hwnd, hdc);

    Log::shutdown();
    return 0;
}
