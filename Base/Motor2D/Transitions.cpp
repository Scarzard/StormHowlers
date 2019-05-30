#include "Log.h"
#include "App.h"
#include "Transitions.h"
#include "Render.h"
#include "Scene.h"
#include "Window.h"
#include "EntityManager.h"
#include "Player.h"
#include "Map.h"
#include "Gui.h"
#include "Brofiler\Brofiler.h"
#include "MainMenu.h"
#include "IntroScene.h"
#include <math.h>
#include "SDL\include\SDL_render.h"

Transitions::Transitions()
{
	name.assign("scenechange");
}

Transitions::~Transitions()
{}

bool Transitions::Awake(pugi::xml_node&)
{
	LOG("SceneChange Awake");
	bool ret = true;

	screen = { 0, 0, App->win->width*App->win->scale, App->win->height*App->win->scale };
	return ret;
}

bool Transitions::Start()
{
	LOG("SceneChange Start");
	SDL_SetRenderDrawBlendMode(App->render->renderer, SDL_BLENDMODE_BLEND);
	return true;
}


bool Transitions::Update(float dt)
{
	BROFILER_CATEGORY("SceneChange Update", Profiler::Color::Red);

	if (current_step == fade_step::none)
	{
		return true;
	}
	else if (current_step == fade_step::fade_from_black && to_enable == App->scene)
	{
		if (App->main_menu->active)
			App->main_menu->menu_background->visible = false;
	}
	switch (current_step)
	{
		case fade_step::fade_to_black:
		{
			if (intro == true)
			{
				/*App->audio->CleanUp();
				App->audio->Start();*/
				intro = false;
			}
			if (map == true)
			{
			
				App->scene->currentMap = nextMap;
				LOG("%i", App->scene->currentMap);

				App->map->SwitchMaps(App->scene->map_names[nextMap]);

				fading = false;
				current_step = fade_step::fade_from_black;
			
			}
			else if (main_menu_in == true)
			{

				to_enable->Enable();
				App->gui->Start();
				to_enable->Enable();


				App->main_menu->menu_background->visible = true;

				fading = false;
				current_step = fade_step::fade_from_black;
				
			}
			else if (scene == true)
			{
			
				to_disable->Disable();
				App->gui->CleanUp();
				App->map->CleanUp();
				App->entitymanager->DeleteAllEntities();
				if (to_disable == App->scene)
				{
					App->entitymanager->Disable();
				}
				App->gui->Start();
				to_enable->Enable();
				if (to_enable == App->scene)
				{
					App->entitymanager->Enable();
				}

				if (App->scenechange->ContinueGame)
				{
					App->LoadGame();
				}
				if (main_menu == true)
				{
					App->main_menu->menu_background->visible = false;
				}

				current_step = fade_step::fade_from_black;
			
			}
			else
				current_step = fade_step::fade_from_black;

		}
	
		break;

		case fade_step::fade_from_black:
		{
			if (map == true )
			{
			
			
					current_step = fade_step::none;
					map = false;
			

			}
			else if (scene == true || main_menu == true)
			{
			
					current_step = fade_step::none;
					scene = false;
					main_menu = false;
		
			}
			else if (main_menu_in == true)
			{
				
				current_step = fade_step::none;
				main_menu_in = false;
				
			}
			else
			{
				current_step = fade_step::none;
			}
		}
		break;
	}

	return true;
}

bool Transitions::ChangeMap(int newMap, float time)
{
	bool ret = false;
	if (newMap != -1)
	{
		nextMap = newMap;
	}

	//map = true;
	if (current_step == fade_step::none)
	{
		current_step = fade_step::fade_to_black;
		
		//fadetime = time * 0.5f*10.0f / 2.0;
		fading = true;
		ret = true;
	}

	return ret;
}

bool Transitions::IsChanging() const
{
	return current_step != fade_step::none;
}

bool Transitions::SwitchScene(Module* SceneIn, Module* SceneOut)
{
	bool ret = false;

	if (SceneOut == App->intro)
	{
		intro = true;
	}
	if (SceneIn == App->intro)
	{
		scene = true;
	}
	if (SceneIn == App->scene)
	{
		scene = true;
	}
	if (SceneOut == App->main_menu)
	{
		main_menu = true;
	}
	if (SceneIn == App->main_menu)
	{
		main_menu_in = true;
	}

	if (current_step == fade_step::none)
	{
		current_step = fade_step::fade_to_black;
		to_enable = SceneIn;
		to_disable = SceneOut;
		fading = true;

		ret = true;
	}
	return true;
}

bool Transitions::IsSwitching() const
{
	return (current_step != fade_step::none);
}
