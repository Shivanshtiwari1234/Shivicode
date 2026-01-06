#include "Editor.h"
#include <windows.h>
#include <GL/gl.h>

Editor::Editor()
    : caretRow(0), caretCol(0),
      winW(800), winH(600),
      charW(0), charH(0),
      fontReady(false), fontBase(0)
{
    lines.push_back("");
}

// ----------------------
// Initialize font once
// ----------------------
void Editor::initFont() {
    if (fontReady) return;

    HDC hdc = wglGetCurrentDC();
    if (!hdc) return;

    HFONT font = CreateFontA(
        -16, 0, 0, 0,
        FW_NORMAL,
        FALSE, FALSE, FALSE,
        ANSI_CHARSET,
        OUT_TT_PRECIS,
        CLIP_DEFAULT_PRECIS,
        ANTIALIASED_QUALITY,
        FF_DONTCARE,
        "Consolas"
    );

    SelectObject(hdc, font);

    // Get font metrics for exact caret alignment
    TEXTMETRICA tm{};
    GetTextMetricsA(hdc, &tm);
    charW = static_cast<float>(tm.tmAveCharWidth);
    charH = static_cast<float>(tm.tmHeight);

    fontBase = glGenLists(256);
    wglUseFontBitmapsA(hdc, 0, 256, fontBase);

    fontReady = true;
}

// ----------------------
// Input
// ----------------------
void Editor::onChar(char c) {
    lines[caretRow].insert(lines[caretRow].begin() + caretCol, c);
    caretCol++;
}

void Editor::onBackspace() {
    if (caretCol > 0) {
        lines[caretRow].erase(caretCol - 1, 1);
        caretCol--;
    } else if (caretRow > 0) {
        caretCol = static_cast<int>(lines[caretRow - 1].size());
        lines[caretRow - 1] += lines[caretRow];
        lines.erase(lines.begin() + caretRow);
        caretRow--;
    }
}

void Editor::onEnter() {
    std::string rest = lines[caretRow].substr(caretCol);
    lines[caretRow].erase(caretCol);
    lines.insert(lines.begin() + caretRow + 1, rest);
    caretRow++;
    caretCol = 0;
}

// ----------------------
// Resize
// ----------------------
void Editor::onResize(int w, int h) {
    winW = w;
    winH = h;
    glViewport(0, 0, w, h);
}

// ----------------------
// Render editor
// ----------------------
void Editor::render() {
    if (!fontReady)
        initFont();

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, winW, winH, 0, -1, 1);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glColor3f(0.9f, 0.9f, 0.9f);

    for (size_t i = 0; i < lines.size(); ++i) {
        drawText(10.0f, 10.0f + i * charH, lines[i]);
    }

    drawCaret(
        10.0f + caretCol * charW,
        10.0f + caretRow * charH
    );
}

// ----------------------
// Draw a line of text
// ----------------------
void Editor::drawText(float x, float y, const std::string& text) {
    glRasterPos2f(x, y + charH);
    glListBase(fontBase);

    for (unsigned char c : text) {
        glCallList(fontBase + c);
    }
}

// ----------------------
// Draw caret line
// ----------------------
void Editor::drawCaret(float x, float y) {
    glColor3f(1.0f, 1.0f, 1.0f);
    glBegin(GL_LINES);
        glVertex2f(x, y);
        glVertex2f(x, y + charH);
    glEnd();
}
