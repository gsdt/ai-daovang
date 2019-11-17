#include "constant.h"
#include "boardgame.h"
#include "player.h"
#include "gold.h"
#include "rapidjson/document.h"
#include "rapidjson/filereadstream.h"

#include <iostream>
#include <fstream>

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
    this-> maxE = d["energy"].GetInt();

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

    /*TODO: CHECK IF SEVER RESPONSE  IF GOLD = 0 AND 
    CHANGED LAND (SERVER REPOSONSE WITH JSON AMOUNT = 0 OR DELETE OBJECT)*/
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

player boardgame::get_player(int player_id) {
    for(player p: this->player_list) {
        if(p.id == player_id) {
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

int boardgame::can_move(int player_id, int direction) {
    player p = this->get_player(player_id);
    if(direction == D_UP) {
        if (p.x - 1 < 0)
            return OUT_SIDE;
        int need_E = DAMAGE[trap_list[p.x - 1][p.y]];
        if (p.E > need_E) return OK;
        return NOT_ENERGY;
    }

    if(direction == D_DOWN) {
        if (p.x + 1 >= this->h)
            return OUT_SIDE;
        int need_E = DAMAGE[trap_list[p.x + 1][p.y]];
        if (p.E > need_E) return OK;
        return NOT_ENERGY;
    }

    if(direction == D_RIGHT) {
        if (p.y + 1 >= this->w)
            return OUT_SIDE;
        int need_E = DAMAGE[trap_list[p.x][p.y + 1]];
        std::cout << "------------" << std::endl;
        std::cout << p.E << std::endl;
        std::cout << need_E << std::endl;
        if (p.E > need_E) return OK;
        return NOT_ENERGY;
    }

    if(direction == D_LEFT) {
        if (p.y - 1 < 0)
            return OUT_SIDE;
        int need_E = DAMAGE[trap_list[p.x][p.y - 1]];
        if(p.E > need_E) return OK;
        return NOT_ENERGY;        
    }
}


int boardgame::can_craft(int player_id)
{
    player p = this->get_player(player_id);
    int is_gold = trap_list[p.x][p.y] == O_GOLD;
    if (!is_gold)
        return NOT_GOLD;

    if (p.E > DIG_COST) return OK;
    return NOT_ENERGY;
};

int boardgame::need_to_rest(int player_id){};

void boardgame::rest(int player_id)
{
    // player *p = &(this->player_list[player_id]);
    // p->rest_count++;
    // p->E += this->maxE / (5 - p->rest_count);
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

int boardgame::object_at(int x, int y) {
    return this->trap_list[x][y];
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