#include "mge/sound/SoundChannel.hpp"

namespace Sound
{
    class SoundChannel;

    SoundChannel::SoundChannel(sf::SoundBuffer* buffer)
    {
        _internalSound = new sf::Sound(*buffer);
        _internalSound->setAttenuation(0.0f);
    }

    float SoundChannel::GetVolume()
    {
        return _internalSound->getVolume();
    }

    void SoundChannel::SetVolume(float target)
    {
        _internalSound->setVolume(target * 100.0f);
    }

    bool SoundChannel::IsLooping()
    {
        return _internalSound->getLoop();
    }

    void SoundChannel::SetLoop(bool target)
    {
        _internalSound->setLoop(target);
    }

    void SoundChannel::Play(float vol,bool loop)
    {
        SetVolume(vol);
        SetLoop(loop);
        _internalSound->play();
    }

    void SoundChannel::Pause()
    {
        _internalSound->pause();
    }

    void SoundChannel::Stop()
    {
        _internalSound->stop();
    }

    bool SoundChannel::isClaimed()
    {
        return !(_internalSound->getStatus() == sf::SoundSource::Stopped);
    }
}
