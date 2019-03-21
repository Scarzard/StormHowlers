#include "Defs.h"
#include "Log.h"
#include "App.h"
#include "Input.h"
#include "Textures.h"
#include "Audio.h"
#include "Render.h"
#include "Window.h"
#include "Transitions.h"
#include "Scene.h"
#include "MainMenu.h"
#include "Gui.h"
#include "Fonts.h"
#include "UI_Element.h"

#include <cstdio>
#include "Brofiler\Brofiler.h"

MainMenu::MainMenu() : Module()
{
	name.assign("main_menu");
}

// Destructor
MainMenu::~MainMenu()
{}

// Called before render is available
bool MainMenu::Awake(pugi::xml_node& conf)
{
	LOG("Loading Main Menu");

	menu_bg_file_name = conf.child("menu_bg").attribute("file").as_string("");
	current_track = App->audio->tracks_path[1];
	return true;
}

// Called before the first frame
bool MainMenu::Start()
{
	return true;
}

// Called each loop iteration
bool MainMenu::PreUpdate()
{
	BROFILER_CATEGORY("Main Menu PreUpdate", Profiler::Color::AliceBlue);

	return true;
}

// Called each loop iteration
bool MainMenu::Update(float dt)
{
	BROFILER_CATEGORY("Main Menu Update", Profiler::Color::AliceBlue);

	// -------
	if (App->input->GetKey(SDL_SCANCODE_F8) == KEY_DOWN)
	{
		App->gui->UI_Debug = !App->gui->UI_Debug;
	}
	return true;
}

// Called each loop iteration
bool MainMenu::PostUpdate()
{
	BROFILER_CATEGORY("Main Menu PostUpdate", Profiler::Color::AliceBlue);

	return true;
}

// Called before quitting
bool MainMenu::CleanUp()
{
	LOG("Freeing Main Menu");

	return true;
}