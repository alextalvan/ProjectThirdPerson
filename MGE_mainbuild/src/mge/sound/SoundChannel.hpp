#ifndef SOUNDCHANNEL_H
#define SOUNDCHANNEL_H
#include <SFML/Audio/Sound.hpp>
#include <SFML/Audio/SoundBuffer.hpp>
#include <mge/sound/SoundManager.hpp>

namespace Sound
{
    class SoundManager;

    class SoundChannel
    {
    friend class Sound::SoundManager;
    public:
        void Play(float vol=1.0f,bool loop = false);
        void Pause();
        void Stop();
        void SetVolume(float target);
        float GetVolume();
        bool IsLooping();
        void SetLoop(bool target);
        void SetBuffer(sf::SoundBuffer* const buffer);

    private:
        SoundChannel(sf::SoundBuffer* buffer);
        sf::Sound* _internalSound;
        bool isClaimed();
    };
}

#endif // SOUNDCHANNEL_H
