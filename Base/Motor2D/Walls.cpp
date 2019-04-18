#include "EntityManager.h"
#include "Walls.h"
#include "Scene.h"
#include "Render.h"
#include "Map.h"
#include "Player.h"
#include "Brofiler\Brofiler.h"


Walls::Walls() 
{
	
	
}

Walls::Walls(bool isPlayer1, pair<int, int> pos, Collider Collider, Animation* current_anim) : Building(entityType::WALLS, isPlayer1, pos, Collider)
{
	health = 100;
	position = App->map->MapToWorld(pos.first, pos.second);
	//collider = Collider;
	Current_Animation = current_anim;
}

Walls::~Walls()
{
}

bool Walls::Update(float dt)
{
	BROFILER_CATEGORY("Walls Update", Profiler::Color::SandyBrown);

	Building::Update(dt);
	return true;
}

void Walls::CleanUp()
{
	LOG("---Wall Deleted");
	Building::CleanUp();
}