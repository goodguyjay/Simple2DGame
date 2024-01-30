#include <SFML/Graphics.hpp>
/*#include "Game.h"*/
#include <iostream>

#include "Game.hpp"
#include "Vec2.hpp"

int main() {
    Game g("config.txt");
    g.run();

    /*Vec2 v1(3, -10);
    Vec2 v2(11, 7);

    std::cout << v1.dist(v2) << std::endl;*/


    return 0;
}
