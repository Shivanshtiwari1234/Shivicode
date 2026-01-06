#pragma once
#include <string>
#include <vector>

class Editor {
public:
    Editor();

    // Input
    void onChar(char c);
    void onBackspace();
    void onEnter();

    // Window / layout
    void onResize(int width, int height);

    // Rendering
    void render();

private:
    std::vector<std::string> lines;
    int caretRow;
    int caretCol;

    int winW;
    int winH;

    float charW;      // measured from font metrics
    float charH;

    bool fontReady;
    unsigned int fontBase;

    void initFont();
    void drawText(float x, float y, const std::string& text);
    void drawCaret(float x, float y);
};
