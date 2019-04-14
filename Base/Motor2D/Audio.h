#ifndef __Audio_H__
#define __Audio_H__

#include "Module.h"
#include "Timer.h"

#define DEFAULT_MUSIC_FADE_TIME 2.0f

struct _Mix_Music;
struct Mix_Chunk;

enum FX
{
	MIN10 = 1,
	MIN5,
	MIN1,
	TIMERSTART,
	TIMERSTOP,
	MIN3
};

class Audio : public Module
{
public:

	Audio();

	// Destructor
	virtual ~Audio();

	// Called before render is available
	bool Awake(pugi::xml_node&);

	// Called every frame 
	bool Update(float dt);

	// Called before quitting
	bool CleanUp();

	// Play a music file
	bool PlayMusic(const char* path, float fade_time = DEFAULT_MUSIC_FADE_TIME);

	// Load a WAV in memory
	unsigned int LoadFx(const char* path);

	// Play a previously loaded WAV
	bool PlayFx(unsigned int fx, int repeat = 0);

	void AdjustMusicVol(int value);
	void AdjustSoundVol(int value);
	void PauseMusic();
	void FadeOut(const char* path);
	void FadeIn();

	bool song1played = false;
	bool song2played = false;
	bool fading_in = false;
	bool fading_out = false;
	bool reading_time = true;

	int fade_time = 0;

public:
	string			folder_music;
	string			folder_fx;
	vector<string>	tracks_path;
	vector<string>	fx_path;

private:

	_Mix_Music*			music = NULL;
	list<Mix_Chunk*>	fx;

	string track;
};

#endif // __Audio_H__