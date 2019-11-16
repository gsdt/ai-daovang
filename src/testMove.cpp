#include <iostream>
#include "rapidjson/document.h" // will include "rapidjson/rapidjson.h"
#include <string>
#include <fstream>
#include "rapidjson/filereadstream.h"
#include <cstdio>
#include "player.h"
#include "boardgame.h"

using namespace rapidjson;
using namespace std::chrono;

std::string read_all(std::string filename)
{
    std::ifstream fs(filename);
    std::string content((std::istreambuf_iterator<char>(fs)),
                        (std::istreambuf_iterator<char>()));
    fs.close();
    return content;
}

int main()
{
    // auto start = high_resolution_clock::now();

    std::string dataStartGame = read_all("data/dataStartGame.json");
    boardgame board(dataStartGame);
    board.show();

    //test can dic
    std::cout << "TEST: can dig?" << std::endl;
    std::cout << board.can_dig(2) << std::endl;

    std::string dataCheckCanDig = read_all("data/testCanDig.json");
    board.update(dataCheckCanDig);
    board.show();

    //test can dic
    std::cout << "TEST: can dig?" << std::endl;
    std::cout << board.can_dig(1) << std::endl;
    std::cout << board.can_dig(2) << std::endl;
    std::cout << board.can_dig(3) << std::endl;
    std::cout << board.can_dig(0) << std::endl;

    //Test check move
    // std::cout << "TEST: can move?" << std::endl;
    // std::cout << board.can_move_left(1)<<std::endl;
    // std::cout << board.can_move_right(1)<<std::endl;
    // std::cout << board.can_move_up(1)<<std::endl;
    // std::cout << board.can_move_down(1)<<std::endl;

    // auto stop = high_resolution_clock::now();
    // auto duration = duration_cast<microseconds>(stop - start);
    // std::cout << "Time to read data from server: " << duration.count() << std::endl;

}
