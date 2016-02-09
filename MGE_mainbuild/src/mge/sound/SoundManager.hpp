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
        Sound::SoundChannel* const LoadMusic(std::string fileName);


    private:
         SoundManager();
        ~SoundManager();

        static SoundManager* _instance;
        std::vector<SoundChannel*> _sfxChannels;
        Sound::SoundChannel* _musicChannel = NULL;
        Sound::SoundCacher* _cache;
    };

    void PlaySFX(std::string fileName,float vol = 1.0f);
    void PlayMusic(std::string fileName,float vol =1.0f,bool loop = true);
}


#endif // SOUNDMANAGER_H
