#ifndef SOUNDMANAGER_H
#define SOUNDMANAGER_H
#include <vector>
#include <SFML/Audio/Sound.hpp>
#include <SFML/Audio/SoundBuffer.hpp>
#include <SFML/Audio/Music.hpp>
#include <mge/sound/SoundCacher.hpp>
#include <mge/sound/SoundChannel.hpp>

namespace Sound
{
    class SoundChannel;

    class SoundManager
    {
    public:
        static SoundManager* const GetSingleton();
        Sound::SoundChannel* const LoadSFX(std::string fileName);
        sf::Music* const getMusicChannel();


    private:
         SoundManager();
        ~SoundManager();

        static SoundManager* _instance;
        std::vector<SoundChannel*> _sfxChannels;
        sf::Music* _musicChannel;
        Sound::SoundCacher* _cache;
    };
}


#endif // SOUNDMANAGER_H
