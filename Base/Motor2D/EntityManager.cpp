#include "EntityManager.h"
#include "App.h"
#include "Audio.h"
#include "Render.h"
#include "Pathfinding.h"
#include "Scene.h"
#include "Transitions.h"
#include "Entity.h"
#include "Player.h"
#include "Textures.h"
#include "Barracks.h"
#include "CommandCenter.h"
#include "DefenseAOE.h"
#include "DefenseTarget.h"
#include "MainDefense.h"
#include "Mines.h"
#include "Townhall.h"
#include "Walls.h"
#include "Soldier.h"
#include "Building.h"
#include "Troop.h"

#include "Brofiler\Brofiler.h"
#include "PugiXml/src/pugixml.hpp"

EntityManager::EntityManager()
{
	name.append("entitymanager");
}

EntityManager::~EntityManager()
{
}

bool EntityManager::Awake(pugi::xml_node &config)
{
	bool ret = true;

	/*pugi::xml_document	config_file;
	pugi::xml_node config;
	config = App->LoadConfig(config_file);
	config = config.child("entitymanager").child(s_type).child(&name[0]);*/

	folder.append(config.child("folder").child_value());
	texture_path = config.child("sprite_sheet").attribute("source").as_string();
	entitiesTextures = vector<SDL_Texture*>(Entity::entityType::WAR_HOUND, nullptr);

	return ret;
}

bool EntityManager::Start()
{
	bool ret = true;
	for (int i = Entity::entityType::TOWNHALL; i < Entity::entityType::WAR_HOUND; i++) {
		string n = GetName(Entity::entityType(i));
		n += "_anim.png";

		entitiesTextures[i] = App->tex->Load(PATH(folder.data(), n.data()));
	}

	return ret;
}

bool EntityManager::Update(float dt)
{
	BROFILER_CATEGORY("EntityManager Update", Profiler::Color::Blue);

	bool ret = true;

	if (App->scene->pause == false)
	{
		// Player 1 Buildings
		list<Building*>::const_iterator tmp = App->player1->buildings.begin();
		while (tmp != App->player1->buildings.end())
		{
			ret = (*tmp)->Update(dt);
			tmp++;
		}

		// Player 2 Buildings
		tmp = App->player2->buildings.begin();
		while (tmp != App->player2->buildings.end())
		{
			ret = (*tmp)->Update(dt);
			tmp++;
		}
		// Player 1 Troops
		list<Troop*>::const_iterator ttmp = App->player1->troops.begin();
		while (ttmp != App->player1->troops.end())
		{
			ret = (*ttmp)->Update(dt);
			ttmp++;
		}

		// Player 2 Troops
		ttmp = App->player2->troops.begin();
		while (ttmp != App->player2->troops.end())
		{
			ret = (*ttmp)->Update(dt);
			ttmp++;
		}

	}

	return ret;
}

bool EntityManager::PostUpdate()
{
	BROFILER_CATEGORY("EntityManager PostUpdate", Profiler::Color::Blue);

	bool ret = true;

	if (App->scene->pause == false)
	{
		// Player 1 Buildings
		list<Building*>::const_iterator tmp = App->player1->buildings.begin();
		while (tmp != App->player1->buildings.end())
		{
			ret = (*tmp)->PostUpdate();
			tmp++;
		}

		// Player 2 Buildings
		tmp = App->player2->buildings.begin();
		while (tmp != App->player2->buildings.end())
		{
			ret = (*tmp)->PostUpdate();
			tmp++;
		}
		// Player 1 Troops
		list<Troop*>::const_iterator ttmp = App->player1->troops.begin();
		while (ttmp != App->player1->troops.end())
		{
			ret = (*ttmp)->PostUpdate();
			ttmp++;
		}

		// Player 2 Troops
		ttmp = App->player2->troops.begin();
		while (ttmp != App->player2->troops.end())
		{
			ret = (*ttmp)->PostUpdate();
			ttmp++;
		}


	}
	
	return ret;
}

bool EntityManager::CleanUp()
{
	App->tex->UnLoad(texture);
	entity_list.clear();

	return true;
}

void EntityManager::DeleteAllEntities()
{
	// Main entity list
	list<Entity*>::iterator	tmp = entity_list.begin();
	while (tmp != entity_list.end())
	{
		(*tmp)->CleanUp();
		RELEASE(*tmp);
		tmp++;
	}
	entity_list.clear();


	// Player 1 Buildings
	list<Building*>::iterator btmp = App->player1->buildings.begin();
	while (btmp != App->player1->buildings.end())
	{
		(*btmp)->CleanUp();
		RELEASE(*btmp);
		btmp++;
	}
	App->player1->buildings.clear();

	// Player 2 Buildings
	btmp = App->player2->buildings.begin();
	while (btmp != App->player2->buildings.end())
	{
		(*btmp)->CleanUp();
		RELEASE(*btmp);
		btmp++;
	}
	App->player2->buildings.clear();

	// Player 1 Troops
	list<Troop*>::iterator ttmp = App->player1->troops.begin();
	while (ttmp != App->player1->troops.end())
	{
		(*ttmp)->CleanUp();
		RELEASE(*ttmp);
		ttmp++;
	}
	App->player1->troops.clear();

	// Player 2 Troops
	ttmp = App->player2->troops.begin();
	while (ttmp != App->player2->troops.end())
	{
		(*ttmp)->CleanUp();
		RELEASE(*ttmp);
		ttmp++;
	}
	App->player2->troops.clear();

}

bool EntityManager::Draw(float dt) //sprite ordering
{
	bool ret = true;

	list<Entity*>::iterator tmp = entity_list.begin();

	while (tmp != entity_list.end())
	{
		if (entitiesTextures[(*tmp)->type] != nullptr) {

			int posy = (*tmp)->position.second - (*tmp)->Current_Animation->GetCurrentFrame(dt).h;// - ((*tmp)->Current_Animation->GetCurrentFrame(dt).h - (*tmp)->position.second);
			App->render->Blit(entitiesTextures[(*tmp)->type],  (*tmp)->position.first ,posy, &((*tmp)->Current_Animation->GetCurrentFrame(dt)), SDL_FLIP_NONE);
		}
		tmp++;
	}
	return ret;
}

Entity* EntityManager::AddEntity(bool isPlayer1, Entity::entityType type, pair<int, int> position)
{
	Entity* tmp = nullptr;

	switch (type)
	{
	case Entity::entityType::TOWNHALL:
		tmp = new Townhall(isPlayer1, position);
		break;

	case Entity::entityType::MAIN_DEFENSE:
		tmp = new MainDefense(isPlayer1, position);
		break;

	case Entity::entityType::COMMAND_CENTER:
		tmp = new CmdCenter(isPlayer1, position);
		break;

	case Entity::entityType::WALLS:
		tmp = new Walls(isPlayer1, position);
		break;

	case Entity::entityType::DEFENSE_AOE:
		tmp = new DefenseAoe(isPlayer1, position);
		break;

	case Entity::entityType::DEFENSE_TARGET:
		tmp = new DefenseTarget(isPlayer1, position);
		break;

	case Entity::entityType::MINES:
		tmp = new Mines(isPlayer1, position);
		break;

	case Entity::entityType::BARRACKS:
		tmp = new Barracks(isPlayer1, position);
		break;
	case Entity::entityType::SOLDIER:
		tmp = new Soldier(isPlayer1, position);
		break;
	case Entity::entityType::ENGINEER:
		//tmp = new Engineer(isPlayer1, position);
		break;
	case Entity::entityType::TANKMAN:
		//tmp = new Tankman(isPlayer1, position);
		break;
	case Entity::entityType::INFILTRATOR:
		//tmp = new Infiltrator(isPlayer1, position);
		break;
	case Entity::entityType::WAR_HOUND:
		//tmp = new War_hound(isPlayer1, position);
		break;
	}

	if (tmp)
	{
		entity_list.push_back(tmp); // add to main entity list
		if (isPlayer1 == true)
		{
			if (type >= Entity::entityType::TOWNHALL && type <= Entity::entityType::BARRACKS) //if building
			{
				App->player1->buildings.push_back((Building*)tmp);
			}
			else if (type > Entity::entityType::BARRACKS) //if troops
			{
				App->player1->troops.push_back((Troop*)tmp);
			}
			App->player1->collider.dimensions.first = tmp->size.first;
			App->player1->collider.dimensions.second = tmp->size.second;
		}
		else // Player 2 -------------------------------
		{
			if (type >= Entity::entityType::TOWNHALL && type <= Entity::entityType::BARRACKS)
			{
				App->player2->buildings.push_back((Building*)tmp);
			}
			else if (type > Entity::entityType::BARRACKS)
			{
				App->player2->troops.push_back((Troop*)tmp);
			}
			App->player2->collider.dimensions.first = tmp->size.first;
			App->player2->collider.dimensions.second = tmp->size.second;
		}
	}

	
	return tmp;

}

char* EntityManager::GetName(Entity::entityType type) {
	switch (type)
	{
	case Entity::entityType::TOWNHALL:
		return"Townhall";
		break;
	case Entity::entityType::MAIN_DEFENSE:
		return"sentrygun";
		break;
	case Entity::entityType::COMMAND_CENTER:
		return"CommandCenter";
		break;
	case Entity::entityType::WALLS:
		return"Walls";
		break;
	case Entity::entityType::DEFENSE_AOE:
		return"defense_aoe";
		break;
	case Entity::entityType::DEFENSE_TARGET:
		return"Tesla";
		break;
	case Entity::entityType::MINES:
		return"GoldMine";
		break;
	case Entity::entityType::BARRACKS:
		return"Barracks";
		break;
	case Entity::entityType::SOLDIER:
		return"BasicSoldier";
		break;
	case Entity::entityType::TANKMAN:
		return"tankman";
		break;
	case Entity::entityType::INFILTRATOR:
		return"infiltrator";
		break;
	case Entity::entityType::ENGINEER:
		return"engineer";
		break;
	case Entity::entityType::WAR_HOUND:
		return"war_hound";
		break;
	default:
		break;
	}
}
