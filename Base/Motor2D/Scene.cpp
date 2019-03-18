#include "Defs.h"
#include "Log.h"
#include "App.h"
#include "Input.h"
#include "Textures.h"
#include "Render.h"
#include "Window.h"
#include "Map.h"
#include "Pathfinding.h"
#include "Scene.h"
#include "MainMenu.h"
#include "Transitions.h"
#include "EntityManager.h"
#include "Gui.h"
#include "Fonts.h"
#include "UI_Element.h"
#include "Audio.h"

#include "Brofiler\Brofiler.h"
#include <stdio.h>

Scene::Scene() : Module()
{
	name.assign("scene");
}

// Destructor
Scene::~Scene()
{}

// Called before render is available
bool Scene::Awake(pugi::xml_node& config)
{
	bool ret = true;
	LOG("Loading Scene");
	fade_time = config.child("fade_time").attribute("value").as_float();

	for (pugi::xml_node map = config.child("map_name"); map; map = map.next_sibling("map_name"))
	{
		string* data = new string;

		data->assign(map.attribute("name").as_string());
		map_names.push_back(data);
	}

	return ret;
}

// Called before the first frame
bool Scene::Start()
{
	bool ret = false;

	current_track = App->audio->tracks_path.front();
	App->audio->PlayMusic(PATH(App->audio->folder_music.data(), current_track.data()));

	pause = false;
	to_end = false;
	App->map->Load(map_names.front()->data());
	currentMap = 0;

	//walkability map
	int w, h;
	uchar* data = NULL;
	if (App->map->CreateWalkabilityMap(w, h, &data))
	{
		App->pathfinding->SetMap(w, h, data);

		LOG("Create walkability");
	}
	RELEASE_ARRAY(data);
	debug_tex = App->tex->Load("maps/pathfinding.png");

	SpawnEntities();

	return true;
}

// Called each loop iteration
bool Scene::PreUpdate()
{
	BROFILER_CATEGORY("Scene PreUpdate", Profiler::Color::DarkOrange);
	return true;
}

// Called each loop iteration
bool Scene::Update(float dt)
{
	BROFILER_CATEGORY("Scene Update", Profiler::Color::DarkOrange);

	//if (App->input->GetKey(SDL_SCANCODE_F1) == KEY_DOWN) //Start from first level
	//{
	//	Load_level(0);
	//	currentMap = 0;
	//}
	//else if (App->input->GetKey(SDL_SCANCODE_F2) == KEY_DOWN) //Start from beginning of current map
	//{
	//	App->entitymanager->Restart();
	//}
	//else if (App->input->GetKey(SDL_SCANCODE_F3) == KEY_DOWN) //Start from second level
	//{
	//	Load_level(1);
	//	currentMap = 1;
	//}
	//else if (App->input->GetKey(SDL_SCANCODE_F4) == KEY_DOWN) //Go to main menu
	//{
	//	App->scenechange->SwitchScene(App->main_menu, App->scene);
	//}
	//else if (App->input->GetKey(SDL_SCANCODE_F5) == KEY_DOWN) //Save game state
	//{
	//	App->SaveGame();
	//}
	//else if (App->input->GetKey(SDL_SCANCODE_F6) == KEY_DOWN) //Load game state
	//{
	//	App->LoadGame();
	//}
	//else if (App->input->GetKey(SDL_SCANCODE_F8) == KEY_DOWN) //Debug UI
	//{
	//	App->gui->UI_Debug = !App->gui->UI_Debug;
	//}
	//else if (App->input->GetKey(SDL_SCANCODE_F9) == KEY_DOWN) //View colliders
	//{
	//	App->map->debug = !App->map->debug;
	//	App->entitymanager->draw_path = !App->entitymanager->draw_path;
	//}
	//else if (App->input->GetKey(SDL_SCANCODE_F10) == KEY_DOWN) //Godmode
	//{
	//	godmode = !godmode;
	//}
	//else if (App->input->GetKey(SDL_SCANCODE_F11) == KEY_DOWN) //Framerate Cap On/Off
	//{
	//	App->fpsCapON = !App->fpsCapON;
	//}

	//----
	App->map->Draw(dt);
	//App->entitymanager->Draw(dt);

	return true;
}

// Called each loop iteration
bool Scene::PostUpdate()
{
	BROFILER_CATEGORY("Scene PostUpdate", Profiler::Color::DarkOrange);

	bool ret = true;

	if (to_end == true && App->scenechange->IsChanging() == false)
	{
		if (currentMap < map_names.size() - 1)
		{
			ret = App->scenechange->ChangeMap(++currentMap, fade_time);
		}
		else
		{
			currentMap = 0;
			ret = App->scenechange->ChangeMap(currentMap, fade_time);
		}
		to_end = false;
	}

	return ret;
}

// Called before quitting
bool Scene::CleanUp()
{
	LOG("Freeing scene");

	App->tex->UnLoad(debug_tex);
	
	return true;
}


bool Scene::Load(pugi::xml_node& data)
{
	LOG("IsChanging: %i", App->scenechange->IsChanging());

	if (currentMap != data.child("currentMap").attribute("num").as_int())
	{
		LOG("Calling switch maps");
		currentMap = data.child("currentMap").attribute("num").as_int();
		App->map->SwitchMaps(map_names[data.child("currentMap").attribute("num").as_int()]);
	}
	return true;
}

bool Scene::Save(pugi::xml_node& data) const
{
	data.append_child("currentMap").append_attribute("num") = currentMap;
	return true;
}

bool Scene::Load_level(int map)
{
	App->entitymanager->DeleteEntities();
	App->map->SwitchMaps(map_names[map]);
	SpawnEntities();

	return true;
}

void Scene::SpawnEntities()
{
	App->entitymanager->DeleteEntities();
	SpawnEnemies();

	pugi::xml_document	config_file;
	pugi::xml_node		config;

	config = App->LoadConfig(config_file);

	Entity* player = App->entitymanager->AddEntity(Entity::entityType::PLAYER, { 0,0 }, { 0,0 });
	player->Awake(config.child(App->entitymanager->name.data()));
	player->Start();
}


void Scene::SpawnEnemies()
{
	//for (p2List_item<ObjectsGroup*>* object = App->map->data.objLayers.start; object; object = object->next)
	//{
	//	if (object->data->name == ("Enemies"))
	//	{
	//		for (p2List_item<ObjectsData*>* objectdata = object->data->objects.start; objectdata; objectdata = objectdata->next)
	//		{
	//		}
	//	}
	//	if (object->data->name == ("Collision"))
	//	{
	//		for (p2List_item<ObjectsData*>* objectdata = object->data->objects.start; objectdata; objectdata = objectdata->next)
	//		{
	//		}
	//	}
	//}
}