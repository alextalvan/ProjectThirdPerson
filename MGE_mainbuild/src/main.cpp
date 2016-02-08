#include <iostream>

#include "mge/core/AbstractGame.hpp"
#include "mge/MGEDemo.hpp"
//#include "mge/collision/CollisionManager.hpp"

int main()
{
    std::cout << "Starting Game" << std::endl;

    AbstractGame* game = new MGEDemo();
    game->initialize();
    game->run();

	delete game;

    return 0;
}



