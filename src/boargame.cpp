#include "boardgame.h"
#include "player.h"
#include "gold.h"
#include "rapidjson/document.h"
#include "rapidjson/filereadstream.h"

#include <iostream>
#include <fstream>

using namespace rapidjson;

const int DAMAGE[5] = {1, 3, 2, 3, 4};
const int DIG_COST = 5;

const int O_LAND = 0;
const int O_FOREST = 1;
const int O_TRAP = 2;
const int O_SWAMP = 3;
const int O_GOLD = 4;

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
    int x = d["posx"].GetInt();
    int y = d["posy"].GetInt();
    int E = d["energy"].GetInt();

    for (int i = 0; i < n; i++)
    {
        this->player_list.push_back(player(i, x, y, E));
    }
    trap_list.resize(this->h, std::vector<int>(this->w));
    //init data for trap
    Value &array_obstacles = d["gameinfo"]["obstacles"];
    for (int i = 0; i < array_obstacles.Size(); i++)
    {
        int x = array_obstacles[i]["posx"].GetInt();
        int y = array_obstacles[i]["posy"].GetInt();
        int type = array_obstacles[i]["type"].GetInt();
        trap_list[x][y] = DAMAGE[type];
    }

    //init data for gold
    Value &array_gold = d["gameinfo"]["golds"];
    for (int i = 0; i < array_gold.Size(); i++)
    {
        int x = array_gold[i]["posx"].GetInt();
        int y = array_gold[i]["posy"].GetInt();
        int amount = array_gold[i]["amount"].GetInt();
        this->golds_map[{x, y}] = amount;
        this->trap_list[x][y] = DAMAGE[O_GOLD];
    }
}

void boardgame::update(std::string raw_json)
{
    StringStream s(raw_json.c_str());
    Document d;
    d.ParseStream(s);

    //update data for player
    Value &array_player = d["players"];
    for (int i = 0; i < array_player.Size(); i++)
    {
        this->player_list[i].x = array_player[i]["posx"].GetInt();
        this->player_list[i].y = array_player[i]["posy"].GetInt();
        this->player_list[i].score = array_player[i]["score"].GetInt();
        this->player_list[i].E = array_player[i]["energy"].GetInt64();
        this->player_list[i].state = array_player[i]["state"].GetInt64();
    }

    /*TODO: CHECK IF SEVER RESPONSE  IF GOLD = 0 AND 
    CHANGED LAND (SERVER REPOSONSE WITH JSON AMOUNT = 0 OR DELETE OBJECT)*/
    //init data for gold
    Value &array_gold = d["golds"];
    std::map<std::pair<int, int>, int> new_golds_map;
    for (int i = 0; i < array_gold.Size(); i++)
    {
        int x = array_gold[i]["posx"].GetInt();
        int y = array_gold[i]["posy"].GetInt();
        int amount = array_gold[i]["amount"].GetInt();

        new_golds_map[{x, y}] = amount;
    }
    // remove all cleared gold mine
    for (auto it : this->golds_map)
    {
        std::pair<int, int> key = it.first;
        if (new_golds_map.find(key) == new_golds_map.end())
        {
            this->trap_list[key.first][key.second] = DAMAGE[O_LAND];
        }
    }
    golds_map = new_golds_map;

    // update data
    this->T--;
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

bool boardgame::can_move_up(int player_id)
{
    player p = player_list[player_id];
    if (p.x - 1 < 0)
        return false;
    return p.E > trap_list[p.x - 1][p.y];
}
bool boardgame::can_move_down(int player_id)
{
    player p = player_list[player_id];
    if (p.x + 1 >= this->h)
        return false;
    return p.E > trap_list[p.x + 1][p.y];
};
bool boardgame::can_move_right(int player_id)
{
    player p = player_list[player_id];
    if (p.y + 1 >= this->w)
        return false;
    return p.E > trap_list[p.x][p.y + 1];
};
bool boardgame::can_move_left(int player_id)
{
    player p = player_list[player_id];
    if (p.y - 1 < 0)
        return false;
    return p.E > trap_list[p.x][p.y - 1];
};

bool boardgame::can_dig(int player_id)
{
    player p = player_list[player_id];
    int is_gold = trap_list[p.x][p.y] == DAMAGE[O_GOLD];
    if (!is_gold)
        return false;

    return p.E > DIG_COST;
};

bool boardgame::need_to_rest(int player_id){};

void boardgame::rest(int player_id)
{
    player *p = &(this->player_list[player_id]);
    p->rest_count++;
    p->E += this->maxE / (5 - p->rest_count);
};

void boardgame::move_up(int player_id)
{
    this->player_list[player_id].x - 1;
    this->player_list[player_id].rest_count = 0;
    player p = player_list[player_id];
    this->player_list[player_id].E -= trap_list[p.x][p.y];
};
void boardgame::move_down(int player_id)
{
    this->player_list[player_id].x + 1;
    this->player_list[player_id].rest_count = 0;
    player p = player_list[player_id];
    this->player_list[player_id].E -= trap_list[p.x][p.y];
};
void boardgame::move_right(int player_id)
{
    this->player_list[player_id].y + 1;
    this->player_list[player_id].rest_count = 0;
    player p = player_list[player_id];
    this->player_list[player_id].E -= trap_list[p.x][p.y];
};
void boardgame::move_left(int player_id)
{
    this->player_list[player_id].y - 1;
    this->player_list[player_id].rest_count = 0;
    player p = player_list[player_id];
    this->player_list[player_id].E -= trap_list[p.x][p.y];
};

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