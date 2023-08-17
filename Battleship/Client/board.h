#ifndef BOARD_H
#define BOARD_H
#include <vector>
#include "graphics.h"

struct coord {
    short x, y;
};

struct ship {
    coord pos;
    std::string type, asset_h, asset_v;
    short hp, length;
    bool vertical;
};

class board {
    private:
        std::vector <ship> ships;
        std::vector <coord> hits, miss;
        coord target;
    public:
        coord loc;
        board (short x, short y);
        static const int BOARD_SIZE;
        static std::string BOARD;
        void draw();
        void placeShip(ship &s);
        ship *hitOrMiss(const coord &c);
        coord fire();
        bool defeated();
        void addHit(const coord &c);
        void addMiss(const coord &c);
};

#endif