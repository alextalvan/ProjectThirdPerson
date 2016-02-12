#include "mge/util/ResourceCacher.hpp"
#include <iostream>
#include <map>
#include "mge/config.hpp"

namespace Utils
{

    std::map<std::string,sf::Font*> _fontMap;
    std::map<std::string,sf::Texture*> _textureMap;

    sf::Font* LoadFont(std::string fileName)
    {
        sf::Font* ret;
        std::map<std::string,sf::Font*>::iterator pos = _fontMap.find(fileName);

        if(pos==_fontMap.end())
        {
            ret = new sf::Font();
            if(!ret->loadFromFile(config::MGE_FONT_PATH + fileName))
            {
                std::cout<<"Could not load font from file: " + fileName +"\n";
                throw "Exception";
            }
            _fontMap.insert(_fontMap.end(),std::pair<std::string,sf::Font*>(fileName,ret));

        }
        else
        {
            ret = _fontMap[fileName];
        }
        return ret;
    }

    sf::Texture* LoadTexture(std::string fileName)
    {
        sf::Texture* ret;
        std::map<std::string,sf::Texture*>::iterator pos = _textureMap.find(fileName);

        if(pos==_textureMap.end())
        {
            ret = new sf::Texture();
            if(!ret->loadFromFile(config::MGE_TEXTURE_PATH + fileName))
            {
                std::cout<<"Could not load texture from file: " + fileName +"\n";
                throw "Exception";
            }
            _textureMap.insert(_textureMap.end(),std::pair<std::string,sf::Texture*>(fileName,ret));

        }
        else
        {
            ret = _textureMap[fileName];
        }
        return ret;
    }
}
