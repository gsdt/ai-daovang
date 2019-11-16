#include "gold.h"
#include <iostream>

gold::gold(int posx, int posy, int amount)
{
    this->posx = posx;
    this->posy = posy;
    this->amount = amount;
}

void gold::show()
{
    std::cout << "posx = " << this->posx << std::endl;
    std::cout << "posy = " << this->posy << std::endl;
    std::cout << "amount = " << this->amount << std::endl
              << std::endl;
}
