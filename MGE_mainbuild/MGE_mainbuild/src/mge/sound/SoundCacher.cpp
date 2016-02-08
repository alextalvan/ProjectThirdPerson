#include "SoundCacher.hpp"
#include <SFML/Audio/SoundBuffer.hpp>
#include <iostream>
#include <map>

namespace Sound
{
    sf::SoundBuffer* SoundCacher::LoadSoundFile(std::string fileName)
        {

            sf::SoundBuffer* ret;
            std::map<std::string,sf::SoundBuffer*>::iterator pos = _hashTable.find(fileName);

            if(pos==_hashTable.end())
            {
                ret = new sf::SoundBuffer();
                if(!ret->loadFromFile(fileName))
                {
                    std::cout<<"Could not load sound from file: " + fileName +"\n";
                    throw "Exception";
                }

                //_hashTable[fileName] = ret;
                _hashTable.insert(_hashTable.begin(),std::pair<std::string,sf::SoundBuffer*>(fileName,ret));
            }
            else
            {
                ret = _hashTable[fileName];
            }

            return ret;

        }
}
