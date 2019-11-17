#ifndef BOARDGAME_H
#define BOARDGAME_H
#include "player.h"
#include "gold.h"
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
public:
    int w, h, T, my_player_id, maxE;
    boardgame(std::string raw_json);
    void update(std::string raw_json);

    std::vector<gold> golds_list();
    player get_player(int player_id);

    int can_move(int player_id, int direction);
    int can_craft(int player_id);

    int need_to_rest(int player_id);

    void move_up(int player_id);
    void move_down(int player_id);
    void move_right(int player_id);
    void move_left(int player_id);
    void rest(int player_id);
    int object_at(int x, int y);
    void show();


};

#endif
