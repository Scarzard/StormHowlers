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

bool Walls::Start()
{
	bool ret = true;

	list<wall_coordinates*>::iterator item = App->map->data.wall_list.begin();
	while (item != App->map->data.wall_list.end())
	{
		wall_parts* wallpart = new wall_parts();

		wallpart->coordinates.first = (*item)->wall_pair.first;
		wallpart->coordinates.second = (*item)->wall_pair.second;
		wallpart->health = 100;
		
		bool right, left, up, down = false;

		list<wall_coordinates*>::iterator item2 = App->map->data.wall_list.begin();
		while (item2 != App->map->data.wall_list.end())
		{
			
			if ( (*item2)->wall_pair.first-1 == wallpart->coordinates.first && (*item2)->wall_pair.second == wallpart->coordinates.second)
			{
				right = true;
			}
			else if ((*item2)->wall_pair.first + 1 == wallpart->coordinates.first && (*item2)->wall_pair.second == wallpart->coordinates.second)
			{
				left = true;
			}
			else if ((*item2)->wall_pair.first == wallpart->coordinates.first && (*item2)->wall_pair.second-1 == wallpart->coordinates.second)
			{
				up = true;
			}
			else if ((*item2)->wall_pair.first == wallpart->coordinates.first && (*item2)->wall_pair.second + 1 == wallpart->coordinates.second)
			{
				down = true;
			}

			item2++;
		}
		
		if (up)
		{
			wallpart->wall_anim = nullptr;
		}
		else if (down)
		{
			wallpart->wall_anim = nullptr;
		}
		else if (left)
		{
			wallpart->wall_anim = nullptr;
		}
		else if (right)
		{
			wallpart->wall_anim = nullptr;
		}
		else if (down && right)
		{
			wallpart->wall_anim = nullptr;
		}
		else if (down && left)
		{
			wallpart->wall_anim = nullptr;
		}
		else if (up && left)
		{
			wallpart->wall_anim = nullptr;
		}
		else if (up && right)
		{
			wallpart->wall_anim = nullptr;
		}

		wall_parts_list.push_back(wallpart);

		item++;
	}
	



	return ret;
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
	
	list<wall_parts*>::iterator item = wall_parts_list.begin();
	while (item != wall_parts_list.end())
	{
		RELEASE(*item);
		item++;
	}
	wall_parts_list.clear();


	Building::CleanUp();

}

void Walls::LoadAnimations(bool isPlayer1, string path)
{
}
