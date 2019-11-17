#include <string>

const int DAMAGE[5] = {1, 3, 2, 3, 4};
const int DIG_COST = 5;

const int O_LAND = 0;
const int O_FOREST = 1;
const int O_TRAP = 2;
const int O_SWAMP = 3;
const int O_GOLD = 4;

const int OK = 0;
const int OUT_SIDE = 1;
const int NOT_ENERGY = 2;
const int NOT_GOLD = 3;

const int D_LEFT = 0;
const int D_RIGHT = 1;
const int D_UP = 2;
const int D_DOWN = 3;
const int A_FREE = 4;
const int A_CRAFT = 5;


const std::string MOVE_LEFT = "0";
const std::string MOVE_RIGHT = "1";
const std::string MOVE_UP = "2";
const std::string MOVE_DOWN = "3";
const std::string ACT_FREE = "4";
const std::string ACT_CRAFT = "5";