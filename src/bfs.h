#ifndef BFS_H
#define BFS_H

#include "player.h"
#include <vector>
#include <algorithm>
#include <string>


struct step
{
    int x, y;
    int E;
    int prev_x, prev_y;

    step(int x, int y, int E) {
        this->x = x;
        this->y = y;
        this->E = E;
        this->prev_x = -1;
        this->prev_y = -1;
    }

    void set_prev(int x, int y) {
        this->prev_x = x;
        this->prev_y = y;
    }
};


class Compare
{
public:
    bool operator() (step a, step b)
    {
        return a.E > b.E;
    }
};

#endif
