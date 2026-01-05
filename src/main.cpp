#include <windows.h>
#include <gl/GL.h>

#include <array>
#include <string>
#include <stdexcept>

// ================= Input State =================
struct InputState
{
    std::array<bool, 256> keys{};
    std::string text;

    void beginFrame()
    {
        text.clear();
    }

    bool isDown(unsigned vk) const
    {
        return vk < keys.size() && keys[vk];
    }
};

// ================= Editor Buffer =================
struct EditorBuffer
{
    std::string text;

    void insert(const std::string& s)
    {
        text += s;
    }

    void backspace()
    {
        if (!text.empty())
            text.pop_back();
    }
};

// ================= RAII OpenGL Context =================
struct GLContext
{
    HDC   hdc   = nullptr;
    HGLRC hglrc = nullptr;
    HWND  hwnd  = nullptr;

    GLContext(HWND window) : hwnd(window)
    {
        hdc = GetDC(hwnd);
        if (!hdc) throw std::runtime_error("GetDC failed");

        PIXELFORMATDESCRIPTOR pfd{};
        pfd.nSize = sizeof(pfd);
        pfd.nVersion = 1;
        pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL; // single buffer
        pfd.iPixelType = PFD_TYPE_RGBA;
        pfd.cColorBits = 32;

        int pf = ChoosePixelFormat(hdc, &pfd);
        if (!pf || !SetPixelFormat(hdc, pf, &pfd))
            throw std::runtime_error("Pixel format failed");

        hglrc = wglCreateContext(hdc);
        if (!hglrc || !wglMakeCurrent(hdc, hglrc))
            throw std::runtime_error("OpenGL context failed");
    }

    ~GLContext()
    {
        if (hglrc)
        {
            wglMakeCurrent(nullptr, nullptr);
            wglDeleteContext(hglrc);
        }
        if (hdc && hwnd)
            ReleaseDC(hwnd, hdc);
    }

    GLContext(const GLContext&) = delete;
    GLContext& operator=(const GLContext&) = delete;
};

// ================= Window Procedure =================
LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    auto* input = reinterpret_cast<InputState*>(
        GetWindowLongPtr(hwnd, GWLP_USERDATA)
    );

    switch (msg)
    {
    case WM_CREATE:
    {
        auto* cs = reinterpret_cast<CREATESTRUCT*>(lParam);
        SetWindowLongPtr(hwnd, GWLP_USERDATA,
            reinterpret_cast<LONG_PTR>(cs->lpCreateParams));
        return 0;
    }

    case WM_KEYDOWN:
        if (input && wParam < 256)
            input->keys[wParam] = true;
        return 0;

    case WM_KEYUP:
        if (input && wParam < 256)
            input->keys[wParam] = false;
        return 0;

    case WM_CHAR:
        if (input)
            input->text.push_back(static_cast<char>(wParam));
        return 0;

    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    }

    return DefWindowProc(hwnd, msg, wParam, lParam);
}

// ================= Entry Point =================
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int)
{
    try
    {
        const char* CLASS_NAME = "ShivicodeWindow";

        WNDCLASS wc{};
        wc.style = CS_OWNDC;
        wc.lpfnWndProc = WindowProc;
        wc.hInstance = hInstance;
        wc.lpszClassName = CLASS_NAME;

        RegisterClass(&wc);

        InputState input;
        EditorBuffer editor;

        HWND hwnd = CreateWindowEx(
            0,
            CLASS_NAME,
            "Shivicode — Made in C++, made for C++",
            WS_OVERLAPPEDWINDOW | WS_VISIBLE,
            CW_USEDEFAULT, CW_USEDEFAULT,
            800, 600,
            nullptr, nullptr,
            hInstance,
            &input
        );

        if (!hwnd)
            throw std::runtime_error("CreateWindowEx failed");

        GLContext gl(hwnd);

        HFONT font = CreateFontA(
            18, 0, 0, 0,
            FW_NORMAL,
            FALSE, FALSE, FALSE,
            ANSI_CHARSET,
            OUT_DEFAULT_PRECIS,
            CLIP_DEFAULT_PRECIS,
            DEFAULT_QUALITY,
            FF_DONTCARE,
            "Consolas"
        );

        MSG msg{};
        while (GetMessage(&msg, nullptr, 0, 0) > 0)
        {
            input.beginFrame();

            TranslateMessage(&msg);
            DispatchMessage(&msg);

            // ---- Update editor ----
            if (!input.text.empty())
                editor.insert(input.text);

            if (input.isDown(VK_BACK))
                editor.backspace();

            // ---- Render ----
            glClearColor(0.12f, 0.12f, 0.15f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT);
            glFlush();

            SelectObject(gl.hdc, font);
            SetBkMode(gl.hdc, TRANSPARENT);
            SetTextColor(gl.hdc, RGB(220, 220, 220));

            TextOutA(
                gl.hdc,
                10, 10,
                editor.text.c_str(),
                (int)editor.text.size()
            );
        }

        DeleteObject(font);
        return 0;
    }
    catch (...)
    {
        MessageBoxA(nullptr, "Fatal error", "Shivicode", MB_ICONERROR);
        return -1;
    }
}
