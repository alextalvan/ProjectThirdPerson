#include "mge/core/Resources.hpp"
#include "mge/config.hpp"

Resources::Resources()
{

}

Resources::~Resources()
{
    m_Textures.clear();
    m_Fonts.clear();
}
void Resources::loadTexture(std::string name, const sf::String & filename)
{
    sf::Texture* texture = new sf::Texture();
    if (!texture->loadFromFile(config::MGE_TEXTURE_PATH + filename))
        throw ("ResourceManager - Failed to load texture " + filename);
    auto inserted = m_Textures.insert(std::make_pair(name, texture));
    assert(inserted.second);
}

void Resources::loadFont(std::string name, const sf::String & filename)
{
    sf::Font* font = new sf::Font();
    if (!font->loadFromFile(config::MGE_FONT_PATH + filename))
        throw ("ResourceManager - Failed to load font " + filename);
    auto inserted = m_Fonts.insert(std::make_pair(name, font));
    assert(inserted.second);
}

sf::Texture& Resources::getTexture(std::string name) const
{
    auto found = m_Textures.find(name);
    assert(found != m_Textures.end());
    return *found->second;
}

sf::Font& Resources::getFont(std::string name) const
{
    auto found = m_Fonts.find(name);
    assert(found != m_Fonts.end());
    return *found->second;
}
