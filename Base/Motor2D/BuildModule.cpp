#include "Defs.h"
#include "Log.h"

#include "App.h"
#include "Textures.h"
#include "Map.h"
#include "Input.h"
#include "Audio.h"
#include "Window.h"
#include "EntityManager.h"
#include "BuildModule.h"

#include "Brofiler\Brofiler.h"


ActionBuild::ActionBuild() : Module()
{
	name.assign("action_build");
}

ActionBuild::~ActionBuild()
{
}

bool ActionBuild::Awake(pugi::xml_node& config)
{
	//load buildings
	for (pugi::xml_node build = config.child("buildings"); build; build = build.next_sibling("buildings"))
	{
	}

	return true;
}

bool ActionBuild::PreUpdate()
{
	return true;
}

bool ActionBuild::Update(float dt)
{
	BROFILER_CATEGORY("Build Update", Profiler::Color::DarkOrange);

	return true;
}

bool ActionBuild::PostUpdate()
{
	BROFILER_CATEGORY("Build PostUpdate", Profiler::Color::DarkOrange);

	return true;
}

bool ActionBuild::CleanUp()
{
	LOG("Freeing Build");

	return true;
}

void ActionBuild::CheckBuildingPos()
{

}

void ActionBuild::PlaceBuilding(Entity::entityType type, pair<int,int> position)
{
	App->entitymanager->AddEntity(type, position);
}

void ActionBuild::UpdateWalkabilityMap()
{
}