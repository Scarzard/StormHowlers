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

Walls::~Walls()
{
}

Walls::Walls(bool isPlayer1, pair<int, int> pos) : Building(entityType::WALLS,isPlayer1, pos)
{
	string path = "animation/" + name + ".tmx";
	LoadAnimations(isPlayer1, path.data());
}

bool Walls::Update(float dt)
{
	BROFILER_CATEGORY("Walls Update", Profiler::Color::SandyBrown);

	ChangeAnimation();
	Building::Update(dt);

	return true;
}

void Walls::CleanUp()
{
	LOG("---Wall Deleted");
	Building::CleanUp();

}

void Walls::LoadAnimations(bool isPlayer1, string path)
{
}
