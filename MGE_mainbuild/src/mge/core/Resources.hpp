#pragma once

//STL
#include <memory.h>
#include <assert.h>

//SFML
#include <SFML/Graphics.hpp>

class Resources
{
private:
    std::map<std::string, sf::Texture*> m_Textures;
    std::map<std::string, sf::Font*> m_Fonts;
public:
    Resources();
    ~Resources();
    void loadTexture(std::string name, const sf::String& filename);
    void loadFont(std::string name, const sf::String& filename);
    sf::Texture& getTexture(std::string name) const;
    sf::Font& getFont(std::string name) const;
};
