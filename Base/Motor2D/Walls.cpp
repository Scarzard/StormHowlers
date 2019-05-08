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
	health = health_lv[level];
	position = App->map->MapToWorld(pos.first,pos.second);
	collider = Collider;
	Current_Animation = current_anim;
}

Walls::~Walls()
{
}

bool Walls::Update(float dt)
{
	BROFILER_CATEGORY("Walls Update", Profiler::Color::SandyBrown);
	
	if (fromPlayer1)  // --- Player 1 --------------------------------
	{
		if (health <= 0) //destroyed
		{
			App->player1->UpdateWalkabilityMap(true, collider);
			App->player1->DeleteEntity(this);
		}
	}
	else if (!fromPlayer1) // --- Player 2 ---------------------------
	{
		if (health <= 0) //destroyed
		{
			App->player2->UpdateWalkabilityMap(true, collider);
			App->player2->DeleteEntity(this);
		}
	}
	
	Building::Update(dt);
	return true;
}

void Walls::CleanUp()
{
	LOG("---Wall Deleted");
	//App->entitymanager->Remove_fromList(this);

	/*if (fromPlayer1)
	App->player1->UpdateWalkabilityMap(true,);
	else
	{
	App->player1->UpdateWalkabilityMap(true,);
	}*/

	Building::CleanUp();
}