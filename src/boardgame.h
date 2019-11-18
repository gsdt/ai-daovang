#ifndef BOARDGAME_H
#define BOARDGAME_H
#include "player.h"
#include "gold.h"
#include "bfs.h"

#include <string>
#include <vector>
#include <map>
#include <utility>

class boardgame
{
private:
    std::vector<player> player_list;
    std::vector<std::vector<int>> trap_list;
    std::map<std::pair<int,int>, int> golds_map;
    int free_turn = 0;
    BFS bfs_algorithm;
public:
    int w, h, T, my_player_id, maxE;
    boardgame(std::string raw_json);
    void update(std::string raw_json);

    std::vector<gold> golds_list();
    player get_player(int player_id);

    int can_move(int player_id, int direction);
    int can_craft(int player_id);
    int object_at(int x, int y);

    int manhattan_distance(int x1, int y1, int x2, int y2);
    int get_direction(int x1, int y1, int x2, int y2);
    bool can_get_more_gold(int player_id);
    int count_player_at(int x, int y);

    gold best_mine(int player_id);


    // god bless us <3
    int get_best_move(int player_id);


    void move(int player_id, int direction);
    void craft(int player_id);
    void free(int player_id);
    void show();
};

#endif
