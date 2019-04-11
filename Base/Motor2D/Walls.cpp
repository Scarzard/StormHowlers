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

Walls::Walls(bool isPlayer1, pair<int, int> pos) : Entity(entityType::WALLS,isPlayer1, pos)
{
}

bool Walls::Update(float dt)
{
	BROFILER_CATEGORY("Walls Update", Profiler::Color::SandyBrown);

	ChangeAnimation();

	return true;
}

void Walls::CleanUp()
{
	LOG("---Wall Deleted");
}
