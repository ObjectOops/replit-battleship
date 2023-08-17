#include "graphics.h"

void fadeIn(const std::string &s, int x, int y, int r) {
    for (int i {232}; i < 256; ++i) {
        std::cout << "\e[38;5;" << i << 'm';
        display(s, x, y);
        std::cout << std::flush;
        MILLSLEEP(r);
    }
}

void fadeOut(std::string s, int x, int y, int r) {
    for (int i {255}; i >= 232; --i) {
        std::cout << "\e[38;5;" << i << 'm';
        display(s, x, y);
        std::cout << std::flush;
        MILLSLEEP(r);
    }
    for (char &c : s) if (c != '\n') c = ' ';
    display(s, x, y);
}

void linearIn(const std::string &s, int x, int y, int r) {
    std::cout << RESET;
    POS(x, y);
    for (char c : s) {
        if (c == '\n') {
            ++y;
            POS(x, y);
        }else {
            std::cout << c << std::flush;
        }
        MILLSLEEP(r);
    }
}

void linearOut(const std::string &s, int x, int y, int r) {
    linearIn(std::string (s.length(), ' '), x, y, r);
}

void display(const std::string &s, int x, int y) {
    POS(x, y);
    for (char c : s) {
        if (c == '\n') {
            ++y;
            POS(x, y);
        }
        else std::cout << c;
    }
}

assetGroup loadAssets(const std::string &assetFile) {
    std::ifstream fin {assetFile};
    assetGroup assets;
    std::string line, key, asset;
    while (getline(fin, line)) {
        if (line.length() > 0 && line.at(0) == '$') {
            if (!asset.empty()) {
                assets.emplace(key, asset);
                asset.clear();
            }
            key = line;
        }else {
            asset += line + '\n';
        }
    }
    if (!asset.empty()) assets.emplace(key, asset);
    fin.close();
    return assets;
}