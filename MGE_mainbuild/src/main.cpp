#include <iostream>

#include "mge/core/AbstractGame.hpp"
#include "mge/MGEDemo.hpp"


int main()
{
    //std::cout << "Starting Game\n";

    AbstractGame* game = new MGEDemo();
    game->initialize();
    game->run();

	//delete game;

    return 0;
}



