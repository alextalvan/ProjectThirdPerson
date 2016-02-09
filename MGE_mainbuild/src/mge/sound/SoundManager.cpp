#include <mge/sound/SoundManager.hpp>
#include <cstddef>
#include <iostream>

namespace Sound
{
    SoundManager* SoundManager::_instance = NULL;

    SoundManager::SoundManager()
    {
        _cache = new Sound::SoundCacher();
    }

    SoundManager::~SoundManager()
    {
        delete _musicChannel;
    }

    Sound::SoundChannel* const SoundManager::LoadSFX(std::string fileName)
    {
        //cache sound buffer
        sf::SoundBuffer* buffer = _cache->LoadSoundFile(fileName);

        //check if an existing channel that is not claimed can be used
        //SDE::SoundChannel* freeChannel = _sfxChannels->ConditionalSearch([](SDE::SoundChannel* a){return !(a->isClaimed());});
        Sound::SoundChannel* freeChannel = NULL;

        int currentChannelCount = _sfxChannels.size();
        for(int i=0;i<currentChannelCount;++i)
        {
            if(!_sfxChannels[i]->isClaimed())
            {
                freeChannel = _sfxChannels[i];
                break;
            }
        }

        if(freeChannel == NULL)
        {
            //std::cout<<"creating new channel\n";
            Sound::SoundChannel* newChannel = new Sound::SoundChannel(buffer);
            _sfxChannels.push_back(newChannel);
            return newChannel;
        }
        else
        {
            //std::cout<<"using existing channel\n";
            freeChannel->_internalSound->setBuffer(*buffer);
            return freeChannel;
        }
    }

    void PlaySFX(std::string fileName,float vol)
    {
        Sound::SoundManager::GetSingleton()->LoadSFX(fileName)->Play(vol,false);
    }


    Sound::SoundChannel* const SoundManager::LoadMusic(std::string fileName)
    {
        if(_musicChannel == NULL)
        {
            _musicChannel = new Sound::SoundChannel(_cache->LoadSoundFile(fileName));
            return _musicChannel;
        }
        else
        {
            _musicChannel->SetBuffer(_cache->LoadSoundFile(fileName));
            return _musicChannel;
        }
    }

    void PlayMusic(std::string fileName,float vol,bool loop)
    {
        Sound::SoundManager::GetSingleton()->LoadMusic(fileName)->Play(vol,loop);
    }

    SoundManager* const SoundManager::GetSingleton()
    {
        if(_instance == NULL)
        {
            _instance = new SoundManager();
        }

        return _instance;
    }
}
