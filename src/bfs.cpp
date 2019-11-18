#include "bfs.h"
#include "constant.h"

#include <vector>
#include <algorithm>
#include <queue>

struct step
{
    int x, y;
    int E;
    int prev_x, prev_y;
};



void BFS::set_data(std::vector<std::vector<int> > game_map) {
    this->game_map = game_map;
}


int BFS::best_direction(int from_x, int from_y, int to_x, int to_y) {
    // std::queue<step> 
    if(from_y < to_y) {
        return D_RIGHT;
    }
    else if (from_y > to_y) {
        return D_LEFT;
    }
    else if (from_x < to_x) {
        return D_DOWN;
    }
    else return D_UP;
}