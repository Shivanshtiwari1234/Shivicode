#pragma once
#include <chrono>
#include <string>
#include <vector>

class Editor {
public:
    Editor();

    void onChar(char c);
    void onBackspace();
    void onEnter();
    void onResize(int width, int height);
    void render();

    // Test/debug helpers for validating editor state without UI interaction.
    const std::vector<std::string>& debugLines() const { return lines; }
    int debugCaretRow() const { return caretRow; }
    int debugCaretCol() const { return caretCol; }

private:
    std::vector<std::string> lines;
    int caretRow;
    int caretCol;

    int winW;
    int winH;

    float charW;
    float charH;

    bool fontReady;
    unsigned int fontBase;
    bool caretVisible;
    std::chrono::steady_clock::time_point lastBlink;

    static constexpr float kTextPadding = 10.0f;
    static constexpr int kCaretBlinkMs = 500;

    void resetCaretBlink();
    void clampCaret();
    void initFont();
    void drawText(float x, float y, const std::string& text);
    void drawCaret(float x, float y);
};
