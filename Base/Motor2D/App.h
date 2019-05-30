#ifndef __APP_H__
#define __APP_H__

#include "Module.h"
#include "PerfTimer.h"
#include "Timer.h"
#include "PugiXml\src\pugixml.hpp"

// Module
class Window;
class Input;
class Render;
class Textures;
class Audio;
class Pathfinding;
class Scene;
class MainMenu;
class Map;
class Transitions;
class EntityManager;
class Fonts;
class Gui;
class Player;
class MovementManager;
class j1TransitionManager;
class Video;
class IntroScene;

class MainApp
{
public:

	// Constructor
	MainApp(int argc, char* args[]);

	// Destructor
	virtual ~MainApp();

	// Called before render is available
	bool Awake();

	// Called before the first frame
	bool Start();

	// Called each loop iteration
	bool Update();

	// Called before quitting
	bool CleanUp();

	// Add a new module to handle
	void AddModule(Module* module, bool check_active = true);

	// Exposing some properties for reading
	int GetArgc() const;
	const char* GetArgv(int index) const;
	const char* GetTitle() const;
	const char* GetOrganization() const;

	void LoadGame();
	void SaveGame() const;
	void GetSaveGames(list<string>& list_to_fill) const;

	// Load config file
	pugi::xml_node LoadConfig(pugi::xml_document&) const;

private:

	// Call Module before each loop iteration
	void PrepareUpdate();

	// Call Module before each loop iteration
	void FinishUpdate();

	// Call Module before each loop iteration
	bool PreUpdate();

	// Call Module on each loop iteration
	bool DoUpdate();

	// Call Module after each loop iteration
	bool PostUpdate();

	// Load / Save
	bool LoadGameNow();
	bool SavegameNow() const;

public:

	// Module
	Window*					win = NULL;
	Input*					input = NULL;
	Render*					render = NULL;
	Textures*				tex = NULL;
	Audio*					audio = NULL;
	Pathfinding*			pathfinding = NULL;
	Scene*					scene = NULL;
	MainMenu*				main_menu = NULL;
	Map*					map = NULL;
	Transitions*			scenechange = NULL;
	EntityManager*			entitymanager = NULL;
	Fonts*					font = NULL;
	Gui*					gui = NULL;
	Player*					player1 = NULL;
	Player*					player2 = NULL;
	MovementManager*		move_manager = NULL;
	j1TransitionManager*	transitions = NULL;
	Video*			video_manager = NULL;
	IntroScene*		intro = NULL;

	bool			fpsCapON = true;
	bool			vsyncON = false;
	float			dt = 0.0f;

private:

	list<Module*>		module;
	int					argc;
	char**				args;

	string				title;
	string				organization;

	mutable bool		want_to_save = false;
	bool				want_to_load = false;
	string				load_game;
	mutable string		save_game;

	float				avg_fps = 0.0f;
	float				seconds_since_startup = 0.0f;
	float				framerate = 0;
	PerfTimer			ptimer;
	Timer				startup_time;
	Timer				frame_time;
	Timer				last_sec_frame_time;
	uint32				last_sec_frame_count = 0;
	uint32				prev_last_sec_frame_count = 0;
	uint32				last_frame_ms = 0;
	uint32				frames_on_last_update = 0;
	uint64				framerate_cap = 0;
	uint64				frame_count = 0;
	uint64				ms_capped = 0;
};

extern MainApp* App; // No student is asking me about that ... odd :-S

#endif