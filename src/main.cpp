#include <windows.h>
#include <gl/GL.h>
#include <vector>
#include <string>
#include <array>

// ================= Input =================
struct InputState {
    std::array<bool, 256> keys{};
    std::string text;
    void beginFrame() { text.clear(); }
    bool down(int vk) const { return vk < 256 && keys[vk]; }
};

// ================= Editor =================
struct Editor {
    std::vector<std::string> lines{""};
    size_t row = 0, col = 0;

    void insert(char c) {
        lines[row].insert(lines[row].begin() + col, c);
        col++;
    }

    void newline() {
        std::string tail = lines[row].substr(col);
        lines[row].erase(col);
        lines.insert(lines.begin() + row + 1, tail);
        row++;
        col = 0;
    }

    void backspace() {
        if(col>0){
            lines[row].erase(col-1,1);
            col--;
        } else if(row>0){
            col = lines[row-1].size();
            lines[row-1] += lines[row];
            lines.erase(lines.begin()+row);
            row--;
        }
    }
};

// ================= Fullscreen State =================
struct FullscreenState {
    bool active=false;
    DWORD style{};
    RECT rect{};
};

// ================= Window Proc =================
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    auto* input = reinterpret_cast<InputState*>(GetWindowLongPtr(hwnd,GWLP_USERDATA));
    switch(msg){
        case WM_CREATE:
            SetWindowLongPtr(hwnd,GWLP_USERDATA,(LONG_PTR)((CREATESTRUCT*)lParam)->lpCreateParams);
            return 0;
        case WM_KEYDOWN:
            if(input && wParam<256) input->keys[wParam]=true;
            return 0;
        case WM_KEYUP:
            if(input && wParam<256) input->keys[wParam]=false;
            return 0;
        case WM_CHAR:
            if(input) input->text.push_back((char)wParam);
            return 0;
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
    }
    return DefWindowProc(hwnd,msg,wParam,lParam);
}

// ================= OpenGL Font =================
GLuint fontBase = 0;
void initFont(HDC hdc){
    fontBase = glGenLists(256);
    HFONT hFont = CreateFontA(
        -18,0,0,0,FW_NORMAL,FALSE,FALSE,FALSE,
        ANSI_CHARSET,OUT_TT_PRECIS,CLIP_DEFAULT_PRECIS,
        DEFAULT_QUALITY,FF_DONTCARE,"Consolas"
    );
    SelectObject(hdc,hFont);
    wglUseFontBitmaps(hdc,0,256,fontBase);
    DeleteObject(hFont);
}

void renderText(const std::string &text, float x, float y){
    glRasterPos2f(x,y);
    glPushAttrib(GL_LIST_BIT);
    glListBase(fontBase);
    glCallLists((GLsizei)text.size(), GL_UNSIGNED_BYTE, text.c_str());
    glPopAttrib();
}

// ================= Entry =================
int WINAPI WinMain(HINSTANCE hInst,HINSTANCE hPrevInstance,LPSTR lpCmdLine,int nShowCmd){
    const char* CLASS = "ShivicodeWnd";
    WNDCLASS wc{};
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInst;
    wc.lpszClassName = CLASS;
    wc.style = CS_OWNDC;
    RegisterClass(&wc);

    InputState input;
    Editor editor;
    FullscreenState fs;

    HWND hwnd = CreateWindowEx(
        0, CLASS, "Shivicode OpenGL Editor",
        WS_OVERLAPPEDWINDOW|WS_VISIBLE,
        100,100,900,600,
        nullptr,nullptr,hInst,&input
    );

    ShowWindow(hwnd,nShowCmd);
    UpdateWindow(hwnd);

    // OpenGL init
    HDC hdc = GetDC(hwnd);
    PIXELFORMATDESCRIPTOR pfd{};
    pfd.nSize=sizeof(pfd);
    pfd.nVersion=1;
    pfd.dwFlags=PFD_DRAW_TO_WINDOW|PFD_SUPPORT_OPENGL|PFD_DOUBLEBUFFER;
    pfd.iPixelType=PFD_TYPE_RGBA;
    pfd.cColorBits=32;
    int pf = ChoosePixelFormat(hdc,&pfd);
    SetPixelFormat(hdc,pf,&pfd);

    HGLRC rc = wglCreateContext(hdc);
    wglMakeCurrent(hdc,rc);

    // Top-left origin
    glMatrixMode(GL_PROJECTION); glLoadIdentity();
    glOrtho(0,900,600,0,-1,1);
    glMatrixMode(GL_MODELVIEW); glLoadIdentity();

    initFont(hdc); // AFTER window is visible

    MSG msg{};
    static bool f11Prev=false, backPrev=false;

    while(GetMessage(&msg,nullptr,0,0)){
        input.beginFrame();
        TranslateMessage(&msg);
        DispatchMessage(&msg);

        // ---------- F11 fullscreen ----------
        bool f11Now = input.down(VK_F11);
        if(f11Now && !f11Prev){
            if(!fs.active){
                fs.style = GetWindowLong(hwnd,GWL_STYLE);
                GetWindowRect(hwnd,&fs.rect);
                SetWindowLong(hwnd,GWL_STYLE,WS_POPUP|WS_VISIBLE);

                MONITORINFO mi{}; mi.cbSize = sizeof(mi);
                if(GetMonitorInfo(MonitorFromWindow(hwnd,MONITOR_DEFAULTTOPRIMARY),&mi)){
                    SetWindowPos(hwnd,HWND_TOP,mi.rcMonitor.left,mi.rcMonitor.top,
                                 mi.rcMonitor.right-mi.rcMonitor.left,
                                 mi.rcMonitor.bottom-mi.rcMonitor.top,
                                 SWP_NOOWNERZORDER|SWP_FRAMECHANGED|SWP_SHOWWINDOW);
                }
                fs.active = true;
            }else{
                SetWindowLong(hwnd,GWL_STYLE,fs.style);
                SetWindowPos(hwnd,HWND_TOP,fs.rect.left,fs.rect.top,
                             fs.rect.right-fs.rect.left,
                             fs.rect.bottom-fs.rect.top,
                             SWP_NOOWNERZORDER|SWP_FRAMECHANGED|SWP_SHOWWINDOW);
                fs.active = false;
            }
        }
        f11Prev = f11Now;

        // ---------- Input → Editor ----------
        for(char c : input.text){
            if(c=='\r') continue;
            if(c=='\n') editor.newline();
            else editor.insert(c);
        }
        bool backNow = input.down(VK_BACK);
        if(backNow && !backPrev) editor.backspace();
        backPrev = backNow;

        // ---------- Render ----------
        glClearColor(0.10f,0.11f,0.14f,1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        glLoadIdentity();

        float y = 20.0f;
        for(const auto &line : editor.lines){
            renderText(line, 10.0f, y);
            y += 20.0f;
        }

        SwapBuffers(hdc);
    }

    wglMakeCurrent(nullptr,nullptr);
    wglDeleteContext(rc);
    ReleaseDC(hwnd,hdc);

    return 0;
}
