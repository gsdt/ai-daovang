#include <iostream>
#include "rapidjson/document.h" // will include "rapidjson/rapidjson.h"
#include <string>
#include <fstream>
#include "rapidjson/filereadstream.h"
#include <cstdio>
#include "player.h"
#include "boardgame.h"

using namespace rapidjson;

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
    const char json[] = "{\"playerId\":1,\"posx\":1,\"posy\":1,\"energy\":50,\"gameinfo\":{\"numberOfPlayers\":4,\"width\":10,\"height\":10,\"steps\":50,\"golds\":[{\"posx\":1,\"posy\":2,\"amount\":500},{\"posx\":3,\"posy\":4,\"amount\":500},{\"posx\":5,\"posy\":2,\"amount\":500},{\"posx\":7,\"posy\":5,\"amount\":500}],\"obstacles\":[{\"type\":0,\"posx\":4,\"posy\":4},{\"type\":1,\"posx\":5,\"posy\":5},{\"type\":2,\"posx\":6,\"posy\":6},{\"type\":3,\"posx\":7,\"posy\":7}]}}";

    std::string dataStartGame = read_all("data/dataStartGame.json");
    boardgame napData(dataStartGame);
    napData.show();

    //Test Update Data
    std::cout << "===============================================1: " << std::endl;
    std::string dataTurn1 = read_all("data/dataAfterTurn1.json");
    napData.update(dataTurn1);
    napData.show();

    std::cout << "===============================================2: " << std::endl;
    std::string dataTurn2 = read_all("data/dataAfterTurn2.json");
    napData.update(dataTurn2);
    napData.show();

    std::cout << "===============================================3: " << std::endl;
    std::string dataTurn3 = read_all("data/dataAfterTurn3.json");
    napData.update(dataTurn3);
    napData.show();

    std::cout << "===============================================4: " << std::endl;
    std::string dataTurn4 = read_all("data/dataAfterTurn4.json");
    napData.update(dataTurn4);
    napData.show();

    std::cout << "===============================================5: " << std::endl;
    std::string dataTurn5 = read_all("data/dataAfterTurn5.json");
    napData.update(dataTurn5);
    napData.show();

    std::cout << "===============================================5: " << std::endl;
    std::string dataTurn6 = read_all("data/dataAfterTurn6.json");
    napData.update(dataTurn6);
    napData.show();

    //Test check move
    

}
