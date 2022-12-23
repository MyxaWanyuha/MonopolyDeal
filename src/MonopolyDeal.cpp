#include <iostream>
#include "Game.h"

int main()
{
    Monopoly::Game g;
    g.Init(2);
    g.BeginTurn();
    auto data = g.GetAllData();
    std::cout << data;
    // Current player has cards:
    // 1. ...
    // 2. ...
    // wait input 0 or number
    
}
