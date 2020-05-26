#include "sound.hpp"

Sound::Sound():
    m_mute(true)
{
    if (Mix_OpenAudio(44000, MIX_DEFAULT_FORMAT, MIX_DEFAULT_CHANNELS, 1024)<0)
    {
        std::cerr << Mix_GetError();
        exit(1);
    }
    if (!Mix_Init(MIX_INIT_OGG))
    {
        std::cerr << "Erreur lors de l'initialisation OGG";
        exit(1);
    }

    music["world1_1"] = Mix_LoadMUS("data/music/overworld.wav");
    music["world1_1 speedup"] = Mix_LoadMUS("data/music/overworld_speed.wav");
    music["invincible"] = Mix_LoadMUS("data/music/invincible.ogg");
    music["death"] = Mix_LoadMUS("data/sound/death.wav");
    music["game_over"] = Mix_LoadMUS("data/music/gameover.wav");
    music["flag"] = Mix_LoadMUS("data/music/lvlend.wav");
    music["logo"] = Mix_LoadMUS("data/music/cosmic.mp3");

    sound["small_jump"] = Mix_LoadWAV("data/sound/small_jump.wav");
    sound["big_jump"] = Mix_LoadWAV("data/sound/big_jump.wav");
    sound["bloc_bumped"] = Mix_LoadWAV("data/sound/bloc_bumped.wav");
    sound["brick_broken"] = Mix_LoadWAV("data/sound/bric_broken.wav");
    sound["stomp"] = Mix_LoadWAV("data/sound/stomp.wav");
    sound["coin"] = Mix_LoadWAV("data/sound/coin.wav");
    sound["mushroom"] = Mix_LoadWAV("data/sound/mushroom_appears.wav");
    sound["power_up"] = Mix_LoadWAV("data/sound/eat_mushroom.wav");
    sound["shot"] = Mix_LoadWAV("data/sound/kick.wav");
    sound["one_up"] = Mix_LoadWAV("data/sound/one_up.wav");
    sound["pipe"] = Mix_LoadWAV("data/sound/pipe.wav");
    sound["countdown"] = Mix_LoadWAV("data/sound/count.wav");
    verify();
    Mix_PlayMusic(NULL, 0);
}

void Sound::verify()
{
    for (std::map< std::string, Mix_Music* >::iterator it = music.begin(); it != music.end(); ++it)
        if (it->second == NULL)
            std::cerr << it->first << " : failed to load sound!\n" << Mix_GetError();
    for (std::map< std::string, Mix_Chunk* >::iterator it = sound.begin(); it != sound.end(); ++it)
        if (it->second == NULL)
            std::cerr << it->first << " : failed to load sound!\n" << Mix_GetError();
}

Sound::~Sound()
{
    for (std::map< std::string, Mix_Music* >::iterator it=music.begin(); it != music.end(); ++it)
        Mix_FreeMusic(it->second);

    for (std::map< std::string, Mix_Chunk* >::iterator it=sound.begin(); it != sound.end(); ++it)
        Mix_FreeChunk(it->second);

    Mix_CloseAudio();
}

void Sound::mute()
{
    Mix_VolumeMusic(m_mute?0:MIX_MAX_VOLUME);
    m_mute = !m_mute;
}

void Sound::play(const std::string& key)
{
    if (music.find(key) != music.end())
        Mix_PlayMusic(music[key], (key == "death" or key == "flag" or key == "logo")?0:-1);
    else if (sound.find(key) != sound.end())
        Mix_PlayChannel(-1, sound[key], 0);
}
