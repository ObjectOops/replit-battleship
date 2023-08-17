#ifndef GRAPHICS_H
#define GRAPHICS_H
#include <unordered_map>
#include <iostream>
#include <fstream>
#include <string>
#include <unistd.h>

#define DECTCEM_L CSI "?25l"
#define DECTCEM_H CSI "?25h"
#define GREEN CSI "38;5;46m"
#define RED CSI "38;5;196m"
#define BLUE CSI "38;5;21m"
#define UNDERLINE CSI "4m"
#define CLR CSI "2J\e[H"
#define ITALIC CSI "3m"
#define RESET CSI "m"
#define BOLD CSI "1m"
#define DIM CSI "2m"
#define CSI "\e["

#define COOKED() system("/bin/stty cooked ; /bin/stty echo")
#define RAW() system("/bin/stty raw ; /bin/stty -echo")

#define MEDIUM 50
#define SLOW 125
#define FAST 25

#define POS(x, y) std::cout << "\e[" << y + 1 << ';' << x + 1 << 'H'

#define MILLSLEEP(t) usleep(t * 1000)

#define GETASSET(a, name) a.find(name)->second

typedef std::unordered_map <std::string, std::string> assetGroup;

void fadeIn(const std::string &s, int x, int y, int r);

void fadeOut(const std::string s, int x, int y, int r);

void linearIn(const std::string &s, int x, int y, int r);

void linearOut(const std::string &s, int x, int y, int r);

void display(const std::string &s, int x, int y);

assetGroup loadAssets(const std::string &assetFile);

#endif