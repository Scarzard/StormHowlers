#include "Defs.h"
#include "Log.h"
#include "App.h"
#include "Scene.h"
#include "Audio.h"

#include "SDL/include/SDL.h"
#include "SDL_mixer\include\SDL_mixer.h"
#pragma comment( lib, "SDL_mixer/libx86/SDL2_mixer.lib" )

#define SONG1_BEGIN_TIME 5
#define SONG2_BEGIN_TIME 8

Audio::Audio() : Module()
{
	music = NULL;
	name.assign("audio");
}

// Destructor
Audio::~Audio()
{}

// Called before render is available
bool Audio::Awake(pugi::xml_node& config)
{
	LOG("Loading Audio Mixer");
	bool ret = true;
	SDL_Init(0);

	if (SDL_InitSubSystem(SDL_INIT_AUDIO) < 0)
	{
		LOG("SDL_INIT_AUDIO could not initialize! SDL_Error: %s\n", SDL_GetError());
		active = false;
		ret = true;
	}

	// load support for the JPG and PNG image formats
	int flags = MIX_INIT_OGG;
	int init = Mix_Init(flags);

	if ((init & flags) != flags)
	{
		LOG("Could not initialize Mixer lib. Mix_Init: %s", Mix_GetError());
		active = false;
		ret = true;
	}

	//Initialize SDL_mixer
	if (Mix_OpenAudio(MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT, 2, 2048) < 0)
	{
		LOG("SDL_mixer could not initialize! SDL_mixer Error: %s\n", Mix_GetError());
		active = false;
		ret = true;
	}

	//initialize music and fx
	folder_music.assign(config.child("music").child("folder").child_value());
	folder_fx.assign(config.child("fx").child("folder").child_value());
	LOG("Folder music %s", folder_music.data());
	LOG("Folder fx %s", folder_fx.data());

	pugi::xml_node music_node = config.child("music").child("track");
	int i = 0;
	for (music_node; music_node; music_node = music_node.next_sibling("track"))
	{
		tracks_path.push_back(music_node.child_value());
		LOG("Loading paths %s", tracks_path[i++].data());
	}
	//Load fx data
	pugi::xml_node fx_node = config.child("fx").child("sound");
	int j = 0;
	for (fx_node; fx_node; fx_node = fx_node.next_sibling("sound"))
	{
		fx_path.push_back(fx_node.child_value());
		LOG("Loading fx path %s  i: %d", fx_path[j].data(), j - 1);
		j++;
	}
	
	for (uint i = 0; i < fx_path.size(); i++)
	{
		LoadFx(PATH(folder_fx.data(), fx_path[i].data()));
	}

	SetMusicVolume();
	SetSfxVolume();

	return ret;
}

bool Audio::Update(float dt)
{
	if (App->scene->worldminutes >= SONG1_BEGIN_TIME && App->scene->worldminutes < SONG2_BEGIN_TIME && song1played == false)
	{
		song1played = true;
		fading_out = true;
		track = folder_music + "/MiddleSong.ogg";
		volume_before = Mix_VolumeMusic(-1); 
	}

	else if (App->scene->worldminutes >= SONG2_BEGIN_TIME && song2played == false)
	{
		song2played = true;
		fading_out = true;
		track = folder_music + "/FinalSong.ogg";
		volume_before = Mix_VolumeMusic(-1);
	}

	else if (App->scene->worldminutes == 10 && winscreensong == false)
	{
		winscreensong = true;
		fading_out = true;
		track = folder_music + "/WinScreenSong.ogg";
		volume_before = Mix_VolumeMusic(-1);
	}


	if (reading_time)
	{
		fade_time = App->scene->world_clock.Read();
		reading_time = false;
	}

	if (fading_out)
		FadeOut(track.c_str());
	if (fading_in)
		FadeIn();

	////Fixes Sliders crash
	//if (musicVolume || sfxVolume < 0)
	//	musicVolume = sfxVolume = 0;
	//if (musicVolume || sfxVolume > 100)
	//	musicVolume = sfxVolume = 100;

	return true;
}

// Called before quitting
bool Audio::CleanUp()
{
	if (!active)
		return true;

	LOG("Freeing sound FX, closing Mixer and Audio subsystem");

	if (music != NULL)
	{
		Mix_FreeMusic(music);
	}

	list<Mix_Chunk*>::const_iterator item;

	for (item = fx.begin(); item != fx.end(); item++)
		Mix_FreeChunk(*item);

	fx.clear();

	Mix_CloseAudio();
	Mix_Quit();
	SDL_QuitSubSystem(SDL_INIT_AUDIO);

	return true;
}

// Play a music file
bool Audio::PlayMusic(const char* path, float fade_time)
{
	bool ret = true;

	if (!active)
		return false;

	if (music != NULL)
	{
		if (fade_time > 0.0f)
		{
			//Mix_FadeOutMusic(int(fade_time * 1000.0f));
			//changing_song = true;
		}
		else
		{
			Mix_HaltMusic();
		}

		// this call blocks until fade out is done
		Mix_FreeMusic(music);
	}

	music = Mix_LoadMUS(path);

	if (music == NULL)
	{
		LOG("Cannot load music %s. Mix_GetError(): %s\n", path, Mix_GetError());
		ret = false;
	}
	else
	{
		if (fade_time > 0.0f)
		{
			if (Mix_FadeInMusic(music, -1, (int)(fade_time * 1000.0f)) < 0)
			{
				LOG("Cannot fade in music %s. Mix_GetError(): %s", path, Mix_GetError());
				ret = false;
			}
		}
		else
		{
			if (Mix_PlayMusic(music, -1) < 0)
			{
				LOG("Cannot play in music %s. Mix_GetError(): %s", path, Mix_GetError());
				ret = false;
			}
		}
	}

	LOG("Successfully playing %s", path);
	return ret;
}

// Load WAV
unsigned int Audio::LoadFx(const char* path)
{
	unsigned int ret = 0;

	if (!active)
		return 0;

	Mix_Chunk* chunk = Mix_LoadWAV(path);

	if (chunk == NULL)
	{
		LOG("Cannot load wav %s. Mix_GetError(): %s", path, Mix_GetError());
	}
	else
	{
		fx.push_back(chunk);
		ret = fx.size();
	}

	return ret;
}

// Play WAV
bool Audio::PlayFx(unsigned int id, int repeat)
{
	bool ret = false;

	if (!active)
		return false;

	if (id > 0 && id <= fx.size())
	{
		Mix_PlayChannel(-1, *next(fx.begin(), id - 1), repeat);
	}

	return ret;
}

void Audio::AdjustMusicVol(int value)
{
	Mix_VolumeMusic(value);
}

void Audio::AdjustSoundVol(int value)
{
	Mix_Volume(-1, value);
}

void Audio::PauseMusic()
{
	if (Mix_PausedMusic() > 0)
	{
		Mix_ResumeMusic();
	}
	else
	{
		Mix_PauseMusic();
	}
}

void Audio::FadeOut(const char* path)
{

	int musicVolume = Mix_VolumeMusic(-1);

	if (App->scene->world_clock.Read() - fade_time > 100 && musicVolume > 0)
	{
		Mix_VolumeMusic(musicVolume -= 5);
		reading_time = true;
	}

	if (musicVolume <= 0)
	{
		Mix_FreeMusic(music); //Changing song
		music = Mix_LoadMUS(path);
		Mix_PlayMusic(music, -1);
		fading_out = false;
		fading_in = true;
	}
}

void Audio::FadeIn()
{
	musicVolume = Mix_VolumeMusic(-1);

	if (App->scene->world_clock.Read() - fade_time > 100 && musicVolume < 128)
	{
		Mix_VolumeMusic(musicVolume += 5);
		reading_time = true;
	}

	if (musicVolume >= musicVolume)
		fading_in = false;
}