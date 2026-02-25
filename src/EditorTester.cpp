#include "Editor.h"
#include <iostream>
#include <string>

static int g_failed = 0;

static void expectTrue(bool condition, const std::string& message) {
    if (!condition) {
        std::cerr << "[FAIL] " << message << '\n';
        g_failed++;
    }
}

static void expectEqInt(int actual, int expected, const std::string& label) {
    if (actual != expected) {
        std::cerr << "[FAIL] " << label << " expected=" << expected
                  << " actual=" << actual << '\n';
        g_failed++;
    }
}

static void expectEqStr(const std::string& actual, const std::string& expected, const std::string& label) {
    if (actual != expected) {
        std::cerr << "[FAIL] " << label << " expected=\"" << expected
                  << "\" actual=\"" << actual << "\"\n";
        g_failed++;
    }
}

int main() {
    Editor editor;

    expectEqInt(static_cast<int>(editor.debugLines().size()), 1, "initial line count");
    expectEqStr(editor.debugLines()[0], "", "initial first line");
    expectEqInt(editor.debugCaretRow(), 0, "initial caret row");
    expectEqInt(editor.debugCaretCol(), 0, "initial caret col");

    editor.onBackspace();
    expectEqStr(editor.debugLines()[0], "", "backspace at file start is no-op");
    expectEqInt(editor.debugCaretRow(), 0, "caret row after start backspace");
    expectEqInt(editor.debugCaretCol(), 0, "caret col after start backspace");

    editor.onChar('a');
    editor.onChar('b');
    expectEqStr(editor.debugLines()[0], "ab", "typed text");
    expectEqInt(editor.debugCaretCol(), 2, "caret after typing");

    editor.onEnter();
    expectEqInt(static_cast<int>(editor.debugLines().size()), 2, "line count after enter");
    expectEqStr(editor.debugLines()[0], "ab", "line 0 after enter");
    expectEqStr(editor.debugLines()[1], "", "line 1 after enter");
    expectEqInt(editor.debugCaretRow(), 1, "caret row after enter");
    expectEqInt(editor.debugCaretCol(), 0, "caret col after enter");

    editor.onChar('c');
    expectEqStr(editor.debugLines()[1], "c", "second line text");
    expectEqInt(editor.debugCaretCol(), 1, "caret col after typing in second line");

    editor.onBackspace();
    expectEqStr(editor.debugLines()[1], "", "backspace removes char from second line");
    expectEqInt(editor.debugCaretCol(), 0, "caret col after char backspace");

    editor.onBackspace();
    expectEqInt(static_cast<int>(editor.debugLines().size()), 1, "line merge count");
    expectEqStr(editor.debugLines()[0], "ab", "line merge text");
    expectEqInt(editor.debugCaretRow(), 0, "caret row after line merge");
    expectEqInt(editor.debugCaretCol(), 2, "caret col after line merge");

    expectTrue(g_failed == 0, "all checks");

    if (g_failed == 0) {
        std::cout << "Editor tests passed\n";
        return 0;
    }

    std::cerr << "Editor tests failed: " << g_failed << '\n';
    return 1;
}
