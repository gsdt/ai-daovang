#ifndef BFS_H
#define BFS_H

#include <vector>
#include <algorithm>
#include <string>

class BFS
{
private:
    std::vector<std::vector<int> > game_map;

public:
    void set_data(std::vector<std::vector<int> > game_map);
    int best_direction(int from_x, int from_y, int to_x, int to_y);
};

#endif
