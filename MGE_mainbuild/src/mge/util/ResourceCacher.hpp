#pragma once
#include <SFML/Graphics.hpp>

namespace Utils
{
    ///TextureCache - provides cache support for textures used by sprites

    sf::Font* LoadFont(std::string fileName);
    sf::Texture* LoadTexture(std::string fileName);
}
