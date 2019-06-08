#include <iostream> 
#include <sstream> 
#include <cstdio>

#include "Defs.h"
#include "Log.h"

#include "Window.h"
#include "Input.h"
#include "Render.h"
#include "Textures.h"
#include "Audio.h"
#include "Pathfinding.h"
#include "Scene.h"
#include "MainMenu.h"
#include "Transitions.h"
#include "Map.h"
#include "EntityManager.h"
#include "Fonts.h"
#include "Gui.h"
#include "Player.h"
#include "MovementManager.h"
#include "j1TransitionManager.h"
#include "VideoManager.h"
#include "IntroScene.h"
#include "App.h"
#include "Brofiler\Brofiler.h"

// Constructor
MainApp::MainApp(int argc, char* args[]) : argc(argc), args(args)
{
	PERF_START(ptimer);

	want_to_save = want_to_load = false;

	input = new Input();
	win = new Window();
	render = new Render();
	tex = new Textures();
	audio = new Audio();
	pathfinding = new Pathfinding();
	scenechange = new Transitions();
	map = new Map();
	entitymanager = new EntityManager();
	font = new Fonts();
	gui = new Gui();
	player1 = new Player(true);
	player2 = new Player(false);
	main_menu = new MainMenu();
	scene = new Scene();
	move_manager = new MovementManager();
	transitions = new j1TransitionManager();
	video_manager = new Video();
	intro = new IntroScene();

	// Ordered for awake / Start / Update
	// Reverse order of CleanUp
	AddModule(input);
	AddModule(win);
	AddModule(tex);
	AddModule(audio);
	AddModule(map);
	AddModule(pathfinding);
	AddModule(font);
	AddModule(gui);
	AddModule(main_menu, false);
	AddModule(scene, false);
	AddModule(player1);
	AddModule(player2);
	AddModule(scenechange);
	AddModule(entitymanager);
	AddModule(move_manager);
	AddModule(transitions);
	AddModule(video_manager);
	AddModule(intro);
	
	
	// render last to swap buffer
	AddModule(render);

	PERF_PEEK(ptimer);
}

// Destructor
MainApp::~MainApp()
{
	// release Module
	list<Module*> ::reverse_iterator item = module.rbegin();

	while (item != module.rend())
	{
		RELEASE(*item);
		item++;
	}

	module.clear();
}

void MainApp::AddModule(Module* modules, bool check_active)
{
	modules->Init(check_active);
	module.push_back(modules);
}

// Called before render is available
bool MainApp::Awake()
{
	PERF_START(ptimer);

	pugi::xml_document	config_file;
	pugi::xml_node		config;
	pugi::xml_node		app_config;

	bool ret = false;

	save_game = load_game = "save_game.xml";

	config = LoadConfig(config_file);

	if(config.empty() == false)
	{
		// self-config
		ret = true;
		app_config = config.child("app");
		title.append(app_config.child("title").child_value());
		organization.append(app_config.child("organization").child_value());
		framerate_cap = config.child("app").attribute("framerate_cap").as_int(-1);
		vsyncON = config.child("renderer").child("vsync").attribute("value").as_bool();

		if (framerate_cap > 0)
		{
			ms_capped = 1000 / framerate_cap;
		}
	}

	if(ret == true)
	{
		list<Module*> :: const_iterator item = module.begin();

		while (item != module.end() && ret)
		{
			ret = (*item)->Awake(config.child((*item)->name.data()));
			if (!ret)
			{
				LOG("name: %s", (*item)->name.data());
			}

			item++;
		}
	}

	PERF_PEEK(ptimer);

	return ret;
}

// Called before the first frame
bool MainApp::Start()
{
	PERF_START(ptimer);

	bool ret = true;
	list<Module*> :: const_iterator item = module.begin();

	while(item != module.end() && ret)
	{
		if ((*item)->active)
		{
			ret = (*item)->Start();
		}
		item++;
	}

	PERF_PEEK(ptimer);

	return ret;
}

// Called each loop iteration
bool MainApp::Update()
{
	BROFILER_CATEGORY("App Update", Profiler::Color::DarkTurquoise);

	bool ret = true;
	PrepareUpdate();

	if(input->GetWindowEvent(WE_QUIT) == true/* || input->GetKey(SDL_SCANCODE_ESCAPE)*/)
		ret = false;

	if(ret == true)
		ret = PreUpdate();

	if(ret == true)
		ret = DoUpdate();

	if(ret == true)
		ret = PostUpdate();

	FinishUpdate();
	return ret;
}

// ---------------------------------------------
pugi::xml_node MainApp::LoadConfig(pugi::xml_document& config_file) const
{
	pugi::xml_node ret;
	pugi::xml_parse_result result = config_file.load_file("config.xml");

	if(result == NULL)
		LOG("Could not load map xml file config.xml. pugi error: %s", result.description());
	else
		ret = config_file.child("config");

	return ret;
}

// ---------------------------------------------
void MainApp::PrepareUpdate()
{
	frame_count++;
	last_sec_frame_count++;

	ptimer.Start();
}

// ---------------------------------------------
void MainApp::FinishUpdate()
{
	if (want_to_save == true)
		SavegameNow();

	if (want_to_load == true)
		LoadGameNow();


	// Framerate calculations
	if (last_sec_frame_time.Read() > 1000)
	{
		last_sec_frame_time.Start();
		prev_last_sec_frame_count = last_sec_frame_count;
		last_sec_frame_count = 0;
	}

	avg_fps = float(frame_count) / startup_time.ReadSec();
	seconds_since_startup = startup_time.ReadSec();
	uint32 current_ms_frame = ptimer.ReadMs();
	last_frame_ms = current_ms_frame;

	if (fpsCapON)
	{
		if (ms_capped > 0 && last_frame_ms < ms_capped)
		{
			PerfTimer timer;
			SDL_Delay(ms_capped - last_frame_ms);
			//LOG("We waited for %d milliseconds and got back in %f", ms_capped - last_frame_ms, timer.ReadMs());
		}
	}

	framerate = 1000.0f / ptimer.ReadMs();
	if (App->scene->pause == true)
		dt = 0.0f;
	else
		dt = 1.0f / framerate;


	pair<int, int> pos;
	App->input->GetMousePosition(pos.first, pos.second);
	pair<int, int> mouse = pos;
	pos = App -> render->ScreenToWorld(pos.first, pos.second);
	pos = App->map->WorldToMap(pos.first, pos.second);

	static char title[256];
	
	sprintf_s(title, 256, "FPS: %i | Av.FPS: %.2f | MsLastFrame: %02u ms | Last dt: %.5f | FPS_Cap: %i | Vsync: %i | Mouse: %i, %i | Tile: %i, %i | Time: %u:%u | Zoom %.2f |Camera x:%i y:%i w:%i h:%i | P1Troops: %i",
		prev_last_sec_frame_count, avg_fps, last_frame_ms, dt,  fpsCapON, vsyncON, mouse.first, mouse.second,pos.first,pos.second, scene->worldminutes,scene->worldseconds, render->zoom, render->camera.x, render->camera.y, render->camera.w, render->camera.h, player1->troops.size());


	App->win->SetTitle(title);
}

// Call Module before each loop iteration
bool MainApp::PreUpdate()
{
	bool ret = true;

	list<Module*> :: const_iterator item;
	Module* pModule = NULL;

	for(item = module.begin(); item != module.end() && ret == true; ++item)
	{
		pModule = *item;

		if(pModule->active == false) {
			continue;
		}

		ret = (*item)->PreUpdate();
	}

	return ret;
}

// Call Module on each loop iteration
bool MainApp::DoUpdate()
{
	bool ret = true;
	list<Module*> ::const_iterator item;
	Module* pModule = NULL;

	for(item = module.begin(); item != module.end() && ret == true; ++item)
	{
		pModule = (*item);

		if(pModule->active == false) {
			continue;
		}

		ret = (*item)->Update(dt);
	}

	return ret;
}

// Call Module after each loop iteration
bool MainApp::PostUpdate()
{
	bool ret = true;
	list<Module*> ::const_iterator item;
	Module* pModule = NULL;

	for(item = module.begin(); item != module.end() && ret == true; ++item)
	{
		pModule = (*item);

		if(pModule->active == false) {
			continue;
		}

		ret = (*item)->PostUpdate();
	}

	return ret;
}

// Called before quitting
bool MainApp::CleanUp()
{
	bool ret = true;
	list<Module*>::reverse_iterator item = module.rbegin();

	while(item != module.rend() && ret == true)
	{
		if ((*item)->active)
		{
			ret = (*item)->CleanUp();
		}
		item++;
	}

	return ret;
}

// ---------------------------------------
int MainApp::GetArgc() const
{
	return argc;
}

// ---------------------------------------
const char* MainApp::GetArgv(int index) const
{
	if(index < argc)
		return args[index];
	else
		return NULL;
}

// ---------------------------------------
const char* MainApp::GetTitle() const
{
	return title.data();
}

// ---------------------------------------
const char* MainApp::GetOrganization() const
{
	return organization.data();
}

// Load / Save
void MainApp::LoadGame()
{
	// we should be checking if that file actually exist
	// from the "GetSaveGames" list
	want_to_load = true;
	//load_game.create("%s%s", fs->GetSaveDirectory(), file);
}

// ---------------------------------------
void MainApp::SaveGame() const
{
	// we should be checking if that file actually exist
	// from the "GetSaveGames" list ... should we overwrite ?

	want_to_save = true;
	//save_game.create(file);
}

// ---------------------------------------
void MainApp::GetSaveGames(list<string>& list_to_fill) const
{
	// need to add functionality to file_system module for this to work
}

bool MainApp::LoadGameNow()
{
	bool ret = false;

	pugi::xml_document data;
	pugi::xml_node root;

	pugi::xml_parse_result result = data.load_file(load_game.data());

	if(result != NULL)
	{
		LOG("Loading new Game State from %s...", load_game.data());

		root = data.child("game_state");

		list<Module*>::const_iterator item = module.begin();
		ret = true;

		while(item != module.end() && ret == true)
		{
			//if (item->data->name == "scene") //load current map's last saved state
			//{
			//	root.child("scene").child("currentMap").attribute("num").set_value(scene->currentMap);
			//}
			ret = (*item)->Load(root.child((*item)->name.data()));
			item++;
		}

		data.reset();
		if(ret == true)
			LOG("...finished loading");
		else
			LOG("...loading process interrupted with error on module %s", ((*item) != NULL) ? (*item)->name.data() : "unknown");
	}
	else
		LOG("Could not parse game state xml file %s. pugi error: %s", load_game.data(), result.description());

	want_to_load = false;
	return ret;
}

bool MainApp::SavegameNow() const
{
	bool ret = true;

	LOG("Saving Game State to %s...", save_game.data());

	// xml object were we will store all data first
	pugi::xml_document data;
	pugi::xml_node root;
	root = data.append_child("game_state");

	list<Module*>::const_iterator item = module.begin(); //save all Module
	
	while(item != module.end() && ret == true)
	{
		if ((*item)->active)
		{
			ret = (*item)->Save(root.append_child((*item)->name.data()));
		}
		item++;
	}

	if (ret == true)
	{
		data.save_file(save_game.data()); //save file
		LOG("... finished saving", save_game.data());
	}
	else
		LOG("Save process halted from an error in module %s", ((*item) != NULL) ? (*item)->name.data() : "unknown");

	data.reset();
	want_to_save = false;
	return ret;
}