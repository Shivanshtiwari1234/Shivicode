// Editor.cpp
#include <vector>
#include <string>

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
        if (col > 0) {
            lines[row].erase(col - 1, 1);
            col--;
        } else if (row > 0) {
            col = lines[row - 1].size();
            lines[row - 1] += lines[row];
            lines.erase(lines.begin() + row);
            row--;
        }
    }
};
