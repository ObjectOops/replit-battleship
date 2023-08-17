#include "board.h"
#include "graphics.h"

const int board::BOARD_SIZE {10};

std::string board::BOARD {};

board::board(short x, short y) : target {5, 5}, loc {x, y} {}

void board::draw() {
    std::cout << GREEN;
    display(BOARD, loc.x, loc.y);
    std::cout << RESET;
    for (const ship &s : ships) display(s.vertical ? s.asset_v : s.asset_h, s.pos.x * 2 + loc.x + 3, BOARD_SIZE - s.pos.y + loc.y + 1);
    std::cout << RED;
    for (const coord &c : hits) {
        POS(c.x * 2 + loc.x + 3, BOARD_SIZE - c.y + loc.y + 1);
        std::cout << 'X';
    }
    std::cout << BLUE;
    for (const coord &c : miss) {
        POS(c.x * 2 + loc.x + 3, BOARD_SIZE - c.y + loc.y + 1);
        std::cout << 'O';
    }
    std::cout << RESET;
}

void board::placeShip(ship &s) {
    bool change {true};
    while (true) {
        bool valid {true};
        for (const ship &i : ships) {
            if (i.vertical != s.vertical) {
                const ship &h {i.vertical ? s : i}, &v {i.vertical ? i : s};
                if (h.pos.x <= v.pos.x && 
                        v.pos.x <= h.pos.x + h.length - 1 && 
                        h.pos.y <= v.pos.y && 
                        v.pos.y - v.length + 1 <= h.pos.y) {
                    valid = false;
                    break;
                }
            }else {
                short s1, e1, s2, e2;
                if (!i.vertical && i.pos.y == s.pos.y) {
                    e1 = (s1 = i.pos.x) + i.length - 1;
                    e2 = (s2 = s.pos.x) + s.length - 1;
                    if ((s1 <= s2 && s2 <= e1) || 
                            (s1 <= e2 && e2 <= e1)) {
                        valid = false;
                        break;
                    }
                }else if (i.vertical && i.pos.x == s.pos.x) {
                    s1 = (e1 = i.pos.y) - i.length + 1;
                    s2 = (e2 = s.pos.y) - s.length + 1;
                    if ((s1 <= s2 && s2 <= e1) || 
                            (s1 <= e2 && e2 <= e1)) {
                        valid = false;
                        break;
                    }
                }
            }
        }
        if (change) {
            draw();
            if (!valid) {
                std::cout << RED;
                display(s.vertical ? s.asset_v : s.asset_h, s.pos.x * 2 + loc.x + 3, BOARD_SIZE - s.pos.y + loc.y + 1);
                std::cout << RESET;
            }else {
                display(s.vertical ? s.asset_v : s.asset_h, s.pos.x * 2 + loc.x + 3, BOARD_SIZE - s.pos.y + loc.y + 1);
            }
        }
        change = false;
        char m;
        std::cin >> m;
        switch (m) {
            case 'w': {
                if (s.pos.y < BOARD_SIZE) {++s.pos.y; change = true;}
                break;
            }
            case 'a': {
                if (s.pos.x > 1) {--s.pos.x; change = true;}
                break;
            }
            case 's': {
                if (s.vertical && s.pos.y - s.length + 1 > 1) {--s.pos.y; change = true;}
                else if (!s.vertical && s.pos.y > 1) {--s.pos.y; change = true;}
                break;
            }
            case 'd': {
                if (!s.vertical && s.pos.x + s.length - 1 < BOARD_SIZE) {++s.pos.x; change = true;}
                else if (s.vertical && s.pos.x < BOARD_SIZE) {++s.pos.x; change = true;}
                break;
            }
            case 'r': {
                if (s.vertical && s.pos.x + s.length - 1 <= BOARD_SIZE) {s.vertical = false; change = true;}
                else if (!s.vertical && s.pos.y - s.length + 1 >= 1) {s.vertical = true; change = true;}
                break;
            }
            case 'e': {
                if (valid) {
                    ships.push_back(s);
                    return;
                }
            }
        }
    }
}

ship *board::hitOrMiss(const coord &c) {
    for (ship &s : ships) {
        if (s.vertical && 
                s.pos.x == c.x && 
                s.pos.y - s.length + 1 <= c.y && 
                c.y <= s.pos.y) {
            --s.hp;
            hits.push_back(c);
            return &s;
        }
        else if (!s.vertical && 
                s.pos.y == c.y && 
                s.pos.x <= c.x && 
                c.x <= s.pos.x + s.length - 1) {
            --s.hp;
            hits.push_back(c);
            return &s;
        }
    }
    miss.push_back(c);
    return nullptr;
}

coord board::fire() {
    while (true) {
        draw();
        POS(target.x * 2 + loc.x + 3, BOARD_SIZE - target.y + loc.y + 1);
        std::cout << "🔺";
        char m;
        std::cin >> m;
        switch (m) {
            case 'w': {
                if (target.y < BOARD_SIZE) ++target.y;
                break;
            }
            case 'a': {
                if (target.x > 1) --target.x;
                break;
            }
            case 's': {
                if (target.y > 1) --target.y;
                break;
            }
            case 'd': {
                if (target.x < BOARD_SIZE) ++target.x;
                break;
            }
            case 'f': {
                bool redundant {false};
                for (coord &i : hits) if (i.x == target.x && i.y == target.y) {
                        redundant = true;
                        break;
                }
                for (coord &i : miss) if (i.x == target.x && i.y == target.y) {
                        redundant = true;
                        break;
                }
                if (!redundant) return target;
            }
        }
    }
}

bool board::defeated() {
    for (const ship &s : ships) if (s.hp > 0) return false;
    return true;
}

void board::addHit(const coord &c) {
    hits.push_back(c);
}

void board::addMiss(const coord &c) {
    miss.push_back(c);
}