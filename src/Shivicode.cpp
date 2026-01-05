// Shivicode.cpp
#include <windows.h>
#include <gl/GL.h>
#include <array>
#include <string>
#include <chrono>
#include "Editor.cpp"

// ================= Input =================
struct InputState {
    std::array<bool,256> keys{};
    std::string text;
    void beginFrame() { text.clear(); }
    bool down(int vk) const { return vk < 256 && keys[vk]; }
};

// ================= Globals =================
GLuint fontBase;
bool caretVisible = true;
auto lastBlink = std::chrono::steady_clock::now();

// ================= Window Proc =================
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    auto* input = (InputState*)GetWindowLongPtr(hwnd, GWLP_USERDATA);

    switch (msg) {
        case WM_CREATE:
            SetWindowLongPtr(hwnd, GWLP_USERDATA,
                (LONG_PTR)((CREATESTRUCT*)lParam)->lpCreateParams);
            return 0;

        case WM_KEYDOWN:
            if (input && wParam < 256) input->keys[wParam] = true;
            return 0;

        case WM_KEYUP:
            if (input && wParam < 256) input->keys[wParam] = false;
            return 0;

        case WM_CHAR:
            if (input && wParam != VK_BACK)
                input->text.push_back((char)wParam);
            return 0;

        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
    }
    return DefWindowProc(hwnd, msg, wParam, lParam);
}

// ================= OpenGL Font =================
void initFont(HDC hdc) {
    fontBase = glGenLists(256);
    HFONT font = CreateFontA(
        -18,0,0,0,FW_NORMAL,FALSE,FALSE,FALSE,
        ANSI_CHARSET,OUT_TT_PRECIS,CLIP_DEFAULT_PRECIS,
        DEFAULT_QUALITY,FF_DONTCARE,"Consolas"
    );
    SelectObject(hdc, font);
    wglUseFontBitmaps(hdc, 0, 256, fontBase);
    DeleteObject(font);
}

void renderText(const std::string& text, float x, float y) {
    glRasterPos2f(x, y);
    glListBase(fontBase);
    glCallLists((GLsizei)text.size(), GL_UNSIGNED_BYTE, text.c_str());
}

// ================= Entry =================
int WINAPI WinMain(HINSTANCE hInst, HINSTANCE, LPSTR, int nShowCmd) {

    WNDCLASS wc{};
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInst;
    wc.lpszClassName = "ShivicodeWnd";
    wc.style = CS_OWNDC;
    RegisterClass(&wc);

    InputState input;
    Editor editor;

    HWND hwnd = CreateWindowEx(
        0, "ShivicodeWnd", "Shivicode",
        WS_OVERLAPPEDWINDOW | WS_VISIBLE,
        100,100,900,600,
        nullptr,nullptr,hInst,&input
    );

    ShowWindow(hwnd, nShowCmd);

    HDC hdc = GetDC(hwnd);
    PIXELFORMATDESCRIPTOR pfd{};
    pfd.nSize = sizeof(pfd);
    pfd.nVersion = 1;
    pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
    pfd.iPixelType = PFD_TYPE_RGBA;
    pfd.cColorBits = 32;

    int pf = ChoosePixelFormat(hdc, &pfd);
    SetPixelFormat(hdc, pf, &pfd);

    HGLRC rc = wglCreateContext(hdc);
    wglMakeCurrent(hdc, rc);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0,900,600,0,-1,1);
    glMatrixMode(GL_MODELVIEW);

    initFont(hdc);

    MSG msg{};
    bool running = true;

    while (running) {
        input.beginFrame();

        while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
            if (msg.message == WM_QUIT) {
                running = false;
                break;
            }
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        // caret blink
        auto now = std::chrono::steady_clock::now();
        if (std::chrono::duration_cast<std::chrono::milliseconds>(now - lastBlink).count() > 500) {
            caretVisible = !caretVisible;
            lastBlink = now;
        }

        // text input
        for (char c : input.text) {
            if (c == '\n') editor.newline();
            else editor.insert(c);
            caretVisible = true;
            lastBlink = now;
        }

        // backspace
        if (input.down(VK_BACK)) {
            editor.backspace();
            input.keys[VK_BACK] = false;
            caretVisible = true;
            lastBlink = now;
        }

        // render
        glClearColor(0.10f,0.11f,0.14f,1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        float y = 20.0f;
        for (size_t i = 0; i < editor.lines.size(); i++) {
            renderText(editor.lines[i], 10.0f, y);

            if (caretVisible && i == editor.row) {
                renderText("|", 10.0f + editor.col * 10.0f, y);
            }
            y += 20.0f;
        }

        SwapBuffers(hdc);
        Sleep(1);
    }

    wglMakeCurrent(nullptr, nullptr);
    wglDeleteContext(rc);
    ReleaseDC(hwnd, hdc);
    return 0;
}
