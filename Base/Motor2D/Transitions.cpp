#include "Log.h"
#include "App.h"
#include "Transitions.h"
#include "Render.h"
#include "Scene.h"
#include "Audio.h"
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
	else if (current_step == fade_step::fade_from_black)
	{
		
		if (App->main_menu->active)
			App->main_menu->menu_background->visible = false;
	}

	uint now = SDL_GetTicks() - start_time;
	float normalized = (1.0f < ((float)now / (float)total_time) ? 1.0f : ((float)now / (float)total_time));


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
			if (now >= total_time) //screen->black & map->loaded
			{
				App->scene->currentMap = nextMap;
				LOG("%i", App->scene->currentMap);

				App->map->SwitchMaps(App->scene->map_names[nextMap]);

				total_time += total_time;
				start_time = SDL_GetTicks();
				fading = false;
				current_step = fade_step::fade_from_black;
			}
		}
		else if (main_menu == true)
		{
			if (now >= total_time) //screen->black & map->loaded
			{
				//to_disable->Disable();
				//App->gui->CleanUp();
				
				/*if (to_disable == App->scene)
				{
					App->entitymanager->Disable();
					App->map->CleanUp();
					App->entitymanager->DeleteAllEntities();
				}*/

				to_enable->Enable();
				App->gui->Start();
				to_enable->Enable();

				
				App->main_menu->menu_background->visible = true;
				
				//switchtimer.Start();

				total_time += total_time;
				start_time = SDL_GetTicks();
				fading = false;
				current_step = fade_step::fade_from_black;
			}
		}
		else if (scene == true)
		{
			if (now >= total_time)
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

				switchtimer.Start();

				if (App->scenechange->ContinueGame)
				{
					App->LoadGame();
				}
				if (main_menu == true)
				{
					App->main_menu->menu_background->visible = false;
				}

				total_time += total_time;
				start_time = SDL_GetTicks();
				current_step = fade_step::fade_from_black;
				
			}
		}
		else
		{
			if (now >= total_time)
			{
				total_time += total_time;
				start_time = SDL_GetTicks();
				current_step = fade_step::fade_from_black;
			}
		}
	}break;

	case fade_step::fade_from_black:
	{
		normalized = 1.0f - normalized;

		if (map == true )
		{
			
			if (now >= total_time )
			{
				current_step = fade_step::none;
				map = false;
				
			}

		}
		else if (scene == true || main_menu == true)
		{
			if (now >= total_time)
			{
				current_step = fade_step::none;
				scene = false;
				main_menu = false;
			}
		}
		else
		{

			if (now >= total_time)
			{
				current_step = fade_step::none;
			}
		}

	}break;

	}

	SDL_SetRenderDrawColor(App->render->renderer, 0, 0, 0, (Uint8)(normalized * 255.0f));
	SDL_RenderFillRect(App->render->renderer, &screen);

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
		start_time = SDL_GetTicks();
		total_time = (Uint32)(time*0.5f*1000.0f);
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

bool Transitions::SwitchScene(Module* SceneIn, Module* SceneOut, float time)
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
	if (SceneIn == App->main_menu)
	{
		main_menu = true;
	}
	if (current_step == fade_step::none)
	{
		current_step = fade_step::fade_to_black;
		switchtimer.Start();
		to_enable = SceneIn;
		to_disable = SceneOut;
		start_time = SDL_GetTicks();
		total_time = (Uint32)(time*0.5f*1000.0f);
		//fadetime = time * 0.5f*10.0f / 2.0;
		fading = true;

		ret = true;
	}
	return true;
}

bool Transitions::IsSwitching() const
{
	return (current_step != fade_step::none);
}
