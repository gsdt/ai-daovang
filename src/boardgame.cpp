#include "constant.h"
#include "boardgame.h"
#include "player.h"
#include "gold.h"
#include "bfs.h"
#include "rapidjson/document.h"
#include "rapidjson/filereadstream.h"

#include <iostream>
#include <algorithm>
#include <fstream>
#include <tgmath.h>

using namespace rapidjson;

int turn_rest = 0;

boardgame::boardgame(std::string raw_json)
{
    StringStream s(raw_json.c_str());
    Document d;
    d.ParseStream(s);

    //start game infor
    this->my_player_id = d["playerId"].GetInt();
    this->w = d["gameinfo"]["width"].GetInt();
    this->h = d["gameinfo"]["height"].GetInt();
    this->T = d["gameinfo"]["steps"].GetInt();

    // init value for players
    int n = d["gameinfo"]["numberOfPlayers"].GetInt();
    int x = d["posy"].GetInt();
    int y = d["posx"].GetInt();
    this->maxE = d["energy"].GetInt();

    this->player_list.push_back(player(this->my_player_id, x, y, this->maxE));

    this->trap_list.resize(this->h, std::vector<int>(this->w));

    //init data for trap
    Value &array_obstacles = d["gameinfo"]["obstacles"];
    for (int i = 0; i < array_obstacles.Size(); i++)
    {
        int x = array_obstacles[i]["posy"].GetInt();
        int y = array_obstacles[i]["posx"].GetInt();
        int type = array_obstacles[i]["type"].GetInt();
        this->trap_list[x][y] = type;
    }

    // init data for gold
    Value &array_gold = d["gameinfo"]["golds"];
    for (int i = 0; i < array_gold.Size(); i++)
    {
        int x = array_gold[i]["posy"].GetInt();
        int y = array_gold[i]["posx"].GetInt();
        int amount = array_gold[i]["amount"].GetInt();
        this->golds_map[{x, y}] = amount;
        this->trap_list[x][y] = O_GOLD;
    }

    this->bfs_algorithm.set_data(trap_list);
    this->free_turn = 0;
}

void boardgame::update(std::string raw_json)
{
    StringStream s(raw_json.c_str());
    Document d;
    d.ParseStream(s);

    //update data for player
    Value &array_player = d["players"];
    this->player_list.clear();
    for (int i = 0; i < array_player.Size(); i++)
    {
        int id = array_player[i]["playerId"].GetInt();
        int x = array_player[i]["posy"].GetInt();
        int y = array_player[i]["posx"].GetInt();
        int score = array_player[i]["score"].GetInt();
        int E = array_player[i]["energy"].GetInt64();
        int state = array_player[i]["state"].GetInt64();

        player p(id, x, y, E);
        p.score = score;
        p.state = state;
        this->player_list.push_back(p);
    }
    //init data for gold
    Value &array_gold = d["golds"];
    std::map<std::pair<int, int>, int> new_golds_map;
    for (int i = 0; i < array_gold.Size(); i++)
    {
        int x = array_gold[i]["posy"].GetInt();
        int y = array_gold[i]["posx"].GetInt();
        int amount = array_gold[i]["amount"].GetInt();

        new_golds_map[{x, y}] = amount;
    }
    // remove all cleared gold mine
    for (auto it : this->golds_map)
    {
        std::pair<int, int> key = it.first;
        if (new_golds_map.find(key) == new_golds_map.end())
        {
            this->trap_list[key.first][key.second] = O_LAND;
        }
    }
    golds_map = new_golds_map;
    // update data
    this->T--;
}

player boardgame::get_player(int player_id)
{
    for (player p : this->player_list)
    {
        if (p.id == player_id)
        {
            return p;
        }
    }
}

std::vector<gold> boardgame::golds_list()
{
    std::vector<gold> result;
    for (auto it : this->golds_map)
    {
        result.push_back(
            gold(it.first.first, it.first.second, it.second));
    }
    return result;
}

int boardgame::can_move(int player_id, int direction)
{
    player p = this->get_player(player_id);

    int cx = p.x + DX[direction];
    int cy = p.y + DY[direction];

    if (cx < 0 || cy < 0 || cx >= this->h || cy >= this->w)
    {
        return OUT_SIDE;
    }

    int need_E = DAMAGE[trap_list[cx][cy]];
    if (p.E > need_E)
        return OK;
    return NOT_ENERGY;
}

int boardgame::can_craft(int player_id)
{
    player p = this->get_player(player_id);
    int is_gold = trap_list[p.x][p.y] == O_GOLD;
    if (!is_gold)
        return NOT_GOLD;

    if (p.E > DIG_COST)
        return OK;
    return NOT_ENERGY;
};

int boardgame::object_at(int x, int y)
{
    return this->trap_list[x][y];
}

int boardgame::get_direction(int x1, int y1, int x2, int y2)
{
    for (int d = 0; d < 4; d++)
    {
        int x = x1 + DX[d];
        int y = y1 + DY[d];
        if (x == x1 && y == y1)
            return d;
    }
    std::cout << "Found no direction :(" << std::endl;
    return -1;
}

int boardgame::manhattan_distance(int x1, int y1, int x2, int y2)
{
    return abs(x1 - x2) + abs(y1 - y2);
}

bool boardgame::can_get_more_gold(int player_id)
{
    player p = this->get_player(player_id);
    int d_min = INT32_MAX;
    std::vector<gold> golds = this->golds_list();
    for (gold g : golds)
    {
        int d = this->manhattan_distance(p.x, p.y, g.x, g.y);
        d_min = std::min(d_min, d);
    }
    if (this->T <= d_min)
        return true;
    return false;
}

int boardgame::count_player_at(int x, int y)
{
    int cnt = 0;
    for (player p : this->player_list)
    {
        if (p.x == x && p.y == y)
        {
            cnt++;
        }
    }
    return cnt;
}

gold boardgame::best_mine(int player_id)
{
    player p = this->get_player(player_id);
    std::vector<gold> golds = this->golds_list();
    gold best = golds.back();

    // // debug
    // return best;

    int c_distance = this->manhattan_distance(p.x, p.y, best.x, best.y);

    for (gold g : golds)
    {
        if (g.amount == 0)
            continue;
        int d = this->manhattan_distance(p.x, p.y, g.x, g.y);
        // best.amount     g.amount
        //------------  < ----------
        // c_distance          d
        if (best.amount * d < g.amount * c_distance)
        {
            best = g;
            c_distance = d;
        }

        if (best.amount * d == g.amount * c_distance)
        {
            if (g.amount > best.amount)
            {
                best = g;
            }
        }
    }

    return best;
}

int boardgame::get_best_move(int player_id)
{
    // if(this->free_turn) {
    //     this->free_turn --;
    //     return A_FREE;
    // }

    player p = this->get_player(player_id);

    // if(! this->can_get_more_gold(player_id)) {
    //     return A_FREE;
    // }

    int code;

    code = this->can_craft(player_id);
    if (code == NOT_ENERGY)
    {
        // int amount_gold = this->golds_map[{p.x,p.y}];
        // if(amount_gold == 50) {
        //     return A_FREE;
        // }

        // int num_turn_to_get_full = 3 + 1.0*(this->maxE/5);
        // if(num_turn_to_get_full > this->T) {
        //     this->free_turn = 1;
        //     return A_FREE;
        // }
        // this->free_turn = 2;
        return A_FREE;
    }
    if (code == OK)
    {
        // TODO: think more...
        return A_CRAFT;
    }

    gold g = this->best_mine(player_id);

    // return MOVE_RIGHT;
    return bfs_algorithm.best_direction(p.x, p.y, g.x, g.y);
}

void boardgame::move(int player_id, int direction)
{
    for (int i = 0; i < this->player_list.size(); i++)
    {
        if (this->player_list[i].id == player_id)
        {
            int x = this->player_list[i].x + DX[direction];
            int y = this->player_list[i].y + DY[direction];

            this->player_list[i].x = x;
            this->player_list[i].y = y;

            this->player_list[i].E -= this->trap_list[x][y];
        }
    }
    this->T--;
}

void boardgame::craft(int player_id)
{
    // if()
}

void boardgame::show()
{
    std::cout << "Main_id: " << this->my_player_id << std::endl;
    std::cout << "width: " << this->w << std::endl;
    std::cout << "height: " << this->h << std::endl;
    std::cout << "steps: " << this->T << std::endl;
    std::cout << "player list: " << std::endl;

    for (player p : this->player_list)
    {
        p.show();
    }

    std::cout << "Gold list: " << std::endl;

    for (gold g : this->golds_list())
    {
        g.show();
    }

    std::cout << "show map: " << std::endl;

    for (auto it : this->trap_list)
    {
        for (int e : it)
        {
            std::cout << e << " ";
        }
        std::cout << std::endl;
    }
}