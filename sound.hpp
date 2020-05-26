#ifndef SOUND_HPP
#define SOUND_HPP
#include <string>
#include <map>
#include <SDL_mixer.h>
#include "map.hpp"

class Sound
{
public:
    Sound();
    ~Sound();
    void play(const std::string& key);
    void mute();
private:
    void verify();
    bool m_mute;
    std::map< std::string, Mix_Music* > music;
    std::map< std::string, Mix_Chunk* > sound;
};

#endif // SOUND_HPP
