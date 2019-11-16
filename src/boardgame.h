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
    bool can_move_up(int player_id);
    bool can_move_down(int player_id);
    bool can_move_right(int player_id);
    bool can_move_left(int player_id);
    bool can_dig(int player_id);

    bool need_to_rest(int player_id);

    void move_up(int player_id);
    void move_down(int player_id);
    void move_right(int player_id);
    void move_left(int player_id);
    void rest(int player_id);

    void show();


};

#endif
