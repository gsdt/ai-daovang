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
#include <chrono> 

using namespace rapidjson;
using namespace std::chrono; 


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

    this->trap_map.resize(this->h, std::vector<int>(this->w));

    //init data for trap
    Value &array_obstacles = d["gameinfo"]["obstacles"];
    for (int i = 0; i < array_obstacles.Size(); i++)
    {
        int x = array_obstacles[i]["posy"].GetInt();
        int y = array_obstacles[i]["posx"].GetInt();
        int type = array_obstacles[i]["type"].GetInt();
        this->trap_map[x][y] = type;
    }

    // init data for gold
    Value &array_gold = d["gameinfo"]["golds"];
    for (int i = 0; i < array_gold.Size(); i++)
    {
        int x = array_gold[i]["posy"].GetInt();
        int y = array_gold[i]["posx"].GetInt();
        int amount = array_gold[i]["amount"].GetInt();
        this->golds_map[{x, y}] = amount;
        this->trap_map[x][y] = O_GOLD;
    }

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
            this->trap_map[key.first][key.second] = O_LAND;
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

    int need_E = DAMAGE[trap_map[cx][cy]];
    if (p.E > need_E)
        return OK;
    return NOT_ENERGY;
}

int boardgame::can_craft(int player_id)
{
    player p = this->get_player(player_id);
    int is_gold = trap_map[p.x][p.y] == O_GOLD;
    if (!is_gold)
        return NOT_GOLD;

    if (p.E > DIG_COST)
        return OK;
    return NOT_ENERGY;
};

int boardgame::object_at(int x, int y)
{
    return this->trap_map[x][y];
}

int boardgame::get_direction(int x1, int y1, int x2, int y2)
{
    std::cout << "Getting direction: from " << x1 << ", " << y1 << " to " << x2 << ", " << y2 << std::endl;
    for (int d = 0; d < 4; d++)
    {
        int x = x1 + DX[d];
        int y = y1 + DY[d];
        if (x == x2 && y == y2)
            return d;
    }
    std::cout << "Found no direction :(" << std::endl;
    #ifdef DEBUG
    throw "Found no direction !";
    #endif
    return -1;
}

int boardgame::manhattan_distance(int x1, int y1, int x2, int y2)
{
    return abs(x1 - x2) + abs(y1 - y2);
}

int boardgame::count_turn_from_path(int player_id, std::vector<int> trap_path) {
    player p = this->get_player(player_id);

    int cnt_turn = trap_path.size();
    std::vector<int> pre_sum(cnt_turn+1, 0);

    for(int i=trap_path.size()-1; i>=0; i++) {
        pre_sum[i] = pre_sum[i+1] + trap_path[i];
    }

    for(int i=0; i<trap_path.size(); i++) {
        if(p.E <= trap_path[i]) {
            if(this->maxE *7/12 + p.E <= pre_sum[i]) {
                cnt_turn += 3;
                p.E = this->maxE;
            }
            else if(this->maxE/4 + p.E <= pre_sum[i]) {
                cnt_turn += 2;
                p.E += this->maxE*7/12;
            }
            else {
                p.E += this->maxE/4;
            }
        }
        p.E -= trap_path[i];
    }
    return cnt_turn;
}


bool boardgame::can_get_more_gold(int player_id)
{
    #ifdef DEBUG
    std::cout << "Check if we can get more gold?" <<std::endl;
    #endif
    player p = this->get_player(player_id);
    int d_min = INT32_MAX;
    std::vector<gold> golds = this->golds_list();
    for (gold g : golds)
    {
        if(p.x == g.x && p.y == g.y) {
            d_min = 0;
            continue;
        }
        int need_T = this->manhattan_distance(p.x, p.y, g.x, g.y);
        int need_E = std::get<0>(this->best_direction(p.x, p.y, g.x, g.y)) - p.E;
        
        int t = need_E / this->maxE;
        need_T += t * 3;
        need_E -= need_E - t*this->maxE;

        t = need_E / (this->maxE * 7/12);
        need_T += t * 2;
        need_E -= t * (this->maxE * 7/12);

        t = need_E / (this->maxE/4);
        need_T += t;
        need_E -= t *(this->maxE/4);

        d_min = std::min(d_min, need_T);
    }
    #ifdef DEBUG
    std::cout << "To earn nearest gold, we need " << d_min << " turns. " <<std::endl;
    #endif
    if (this->T >= d_min + 1)
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


    int c_distance = this->manhattan_distance(p.x, p.y, best.x, best.y);
    // int c_distance = this->best_direction(p.x, p.y, best.x, best.y).first;

    for (gold g : golds)
    {
        if (g.amount == 0)
            continue;
        int distance = this->manhattan_distance(p.x, p.y, g.x, g.y);
        if(distance >= this->T) {
            continue;
        }


        int players_count = this->count_player_at(g.x, g.y);

        g.amount = std::max(0, g.amount - distance*players_count*50);
        g.amount /= (players_count + 1);
        #ifdef DEBUG
        std::cout << "Amount at " << g.x << ", " << g.y << ": " << g.amount << std::endl;
        #endif


        int remain_T = this->T - distance;
        g.amount = std::min(remain_T * 50, g.amount);

        // best.amount     g.amount
        //------------  < ----------
        // c_distance      distance
        if (best.amount * distance < g.amount * c_distance)
        {
            best = g;
            c_distance = distance;
        }

        if (best.amount * distance == g.amount * c_distance)
        {
            if (g.amount > best.amount)
            {
                best = g;
            }
        }
    }

    return best;
}

std::tuple<int, std::vector<int>,  int> boardgame::best_direction(int from_x, int from_y, int to_x, int to_y) {
    #ifdef DEBUG
    std::cout << "finding best way from: (" << from_x << ", " << from_y << ") to (" << to_x << ", " << to_y << ")" << std:: endl;
    auto start = high_resolution_clock::now();
    #endif
    std::priority_queue<step, std::vector<step> , Compare > q;

    q.push(step(to_x, to_y, 0));

    int visited[MAX_H][MAX_W];
    std::pair<int,int> best_E[MAX_H][MAX_W];
    std::pair<int,int> trace[MAX_H][MAX_W];
    
    memset(visited, 0, sizeof(visited));
    for(int i=0; i<this->h; i++) {
        for(int j=0; j<this->w; j++) {
            best_E[i][j] = {INT32_MAX, INT32_MAX};
        }
    }

    while(!q.empty()) {
        step before = q.top();
        q.pop();

        if(visited[before.x][before.y]) continue;

        visited[before.x][before.y] = true;
        trace[before.x][before.y] = {before.prev_x, before.prev_y};

        // #ifdef DEBUG
        // std::cout << "bfs: " << before.x << " " << before.y << " " << before.E << std:: endl;
        // #endif

        if(before.x == from_x && before.y == from_y) {
            int distance = before.E + DAMAGE[this->trap_map[to_x][to_y]];
                distance -= DAMAGE[this->trap_map[from_x][from_y]];
            int direction = this->get_direction(before.x, before.y, before.prev_x, before.prev_y);
            
            std::vector<int> trap_path;
            int cx = from_x, cy = from_y;
            while(true) {
                int x = trace[cx][cy].first;
                int y = trace[cx][cy].second;
                trap_path.push_back(this->trap_map[x][y]);

                if(x == to_x && y == to_y) break;
                cx = x;
                cy = y;
            }


            #ifdef DEBUG
            std::cout << "Trace best_direction: ";
            for(int it: trap_path) {
                std::cout << it <<", ";
            }
            std::cout << std::endl;

            auto stop  = high_resolution_clock::now();
            auto duration = duration_cast<microseconds>(stop - start);
            std::cout << "fun best_direction runtime: " << duration.count() << std::endl;
            #endif
            return std::make_tuple(distance, trap_path, direction);
        }

        for(int i=0; i<4; i++) {
            int next_x = before.x + DX[i];
            int next_y = before.y + DY[i];

            if(next_x < 0 || next_y < 0 || next_x >= this->h || next_y >= this->w) {
                continue;
            }

            if(visited[next_x][next_y]) {
                continue;
            }

            int next_E = before.E + DAMAGE[this->trap_map[next_x][next_y]];
            int next_T = before.T + 1;
            std::pair<int,int> cost = {next_T, next_E};

            if(cost < best_E[next_x][next_y]) {
                best_E[next_x][next_y] = cost;
                
                step s(next_x, next_y, next_E);
                s.set_prev(before.x, before.y);
                
                q.push(s);
            }
        }
    }
}


int boardgame::get_best_move(int player_id)
{
    if(this->free_turn) {
        this->free_turn --;
        return A_FREE;
    }

    player p = this->get_player(player_id);

    if(! this->can_get_more_gold(player_id)) {
        return A_FREE;
    }

    int code;

    code = this->can_craft(player_id);
    if (code == NOT_ENERGY)
    {

        #ifdef DEBUG
        std::cout << "Not energy for craft: E = " << p.E << ", T = " << this->T << std::endl;
        #endif
        int amount_gold = this->golds_map[{p.x,p.y}] / this->count_player_at(p.x, p.y);

        int expected_E = p.E + this->maxE/4;
        if(expected_E/5 > amount_gold/50 || expected_E/5 + 2 > this->T || this->maxE - expected_E < 5) {
            return A_FREE;
        }
        expected_E = p.E + this->maxE*7/12;
        if(expected_E/5 > amount_gold/50 || expected_E/5 + 3 > this->T || this->maxE - expected_E < 5) {
            this->free_turn = 1;
            return A_FREE;
        }

        this->free_turn = 2;
        return A_FREE;
    }
    if (code == OK)
    {
        // TODO: think more...
        return A_CRAFT;
    }

    if (code == NOT_GOLD) {
        gold g = this->best_mine(player_id);
        std::tuple<int, std::vector<int>,  int> result = this->best_direction(p.x, p.y, g.x, g.y);
        
        int direction = std::get<2>(result);
        int move_energy = std::get<0>(result);

        int code = this->can_move(player_id, direction);
        
        if(code == OK) {
            return direction;
        }
        if(code == NOT_ENERGY) {
            // free
            if(p.E + this->maxE/4 >= move_energy + std::ceil(1.0*g.amount/50)) {
                return A_FREE;
            }
            if(p.E + this->maxE*7/12 >= move_energy + std::ceil(1.0*g.amount/50)) {
                this->free_turn = 1;
                return A_FREE;
            }
            this->free_turn = 2;
            return A_FREE;
        }
        if(code == OUT_SIDE) {
            std::cout << "Ohh?? sao lai outside?";
            return A_FREE;
        }
    }
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

    for (auto it : this->trap_map)
    {
        for (int e : it)
        {
            std::cout << e << " ";
        }
        std::cout << std::endl;
    }
}