#ifndef SOUNDCACHER_H
#define SOUNDCACHER_H
#include <SFML/Audio/SoundBuffer.hpp>
#include <map>

namespace Sound
{
    class SoundCacher
    {
        private:
            std::map<std::string,sf::SoundBuffer*> _hashTable;

        public:
            sf::SoundBuffer* LoadSoundFile(std::string fileName);
    };
}

#endif // SOUNDCACHER_H
