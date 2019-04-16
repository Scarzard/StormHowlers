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

Walls::Walls(bool isPlayer1, pair<int, int> pos, Collider collider) : Building(entityType::WALLS, isPlayer1, pos, collider)
{
	
	string path = "animation/" + name + ".tmx";
	//LoadAnimations(isPlayer1, path.data());
	LoadWalls();
}

Walls::~Walls()
{
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

bool Walls::LoadWalls()
{
	bool ret = true;

	front = front->LoadAnimation("animation/walls.tmx", "principal_wall");
	side = side->LoadAnimation("animation/walls.tmx", "side_wall");
	corner_down_left = corner_down_left->LoadAnimation("animation/walls.tmx", "corner_east");
	tower = tower->LoadAnimation("animation/walls.tmx", "small_wall");
	corner_down_right = corner_down_right->LoadAnimation("animation/walls.tmx", "corner_north");
	corner_up_left = corner_up_left->LoadAnimation("animation/walls.tmx", "corner_south");
	corner_up_right = corner_up_right->LoadAnimation("animation/walls.tmx", "corner_west");

	if (fromPlayer1)
	{
		list<wall_coordinates*>::iterator item = App->map->data.wall_list.begin();
		while (item != App->map->data.wall_list.end())
		{
			wall_parts* wallpart = new wall_parts();

			wallpart->coordinates.first = (*item)->wall_pair.first;
			wallpart->coordinates.second = (*item)->wall_pair.second;
			wallpart->health = 100;

			bool right = false, left = false, up = false, down = false;

			list<wall_coordinates*>::iterator item2 = App->map->data.wall_list.begin();
			while (item2 != App->map->data.wall_list.end())
			{

				if ((*item2)->wall_pair.first - 1 == wallpart->coordinates.first && (*item2)->wall_pair.second == wallpart->coordinates.second)
				{
					right = true;
				}
				else if ((*item2)->wall_pair.first + 1 == wallpart->coordinates.first && (*item2)->wall_pair.second == wallpart->coordinates.second)
				{
					left = true;
				}
				else if ((*item2)->wall_pair.first == wallpart->coordinates.first && (*item2)->wall_pair.second - 1 == wallpart->coordinates.second)
				{
					down = true;
				}
				else if ((*item2)->wall_pair.first == wallpart->coordinates.first && (*item2)->wall_pair.second + 1 == wallpart->coordinates.second)
				{
					up = true;
				}

				item2++;
			}

			if (down && up)
			{
				//side
				wallpart->wall_anim = side;
			}
			else if (left && right)
			{
				//frontal
				wallpart->wall_anim = front;
			}
			else if (down && right)
			{
				//corner
				wallpart->wall_anim = corner_down_right;
			}
			else if (down && left)
			{
				//corner
				wallpart->wall_anim = corner_down_left;
			}
			else if (up && left)
			{
				//corner
				wallpart->wall_anim = corner_up_left;
			}
			else if (up && right)
			{
				//corner
				wallpart->wall_anim = corner_up_right;
			}
			else if (up || down || left || right)
			{
				//only tower
				wallpart->wall_anim = tower;

			}

			App->entitymanager->wall_parts_list.push_back(wallpart);

			item++;
		}
	}
	else if (fromPlayer1 == false)
	{
		list<wall_coordinates*>::iterator item = App->map->data.wall_list2.begin();
		while (item != App->map->data.wall_list2.end())
		{
			wall_parts* wallpart = new wall_parts();

			wallpart->coordinates.first = (*item)->wall_pair.first;
			wallpart->coordinates.second = (*item)->wall_pair.second;
			wallpart->health = 100;

			bool right = false, left = false, up = false, down = false;

			list<wall_coordinates*>::iterator item2 = App->map->data.wall_list2.begin();
			while (item2 != App->map->data.wall_list2.end())
			{

				if ((*item2)->wall_pair.first - 1 == wallpart->coordinates.first && (*item2)->wall_pair.second == wallpart->coordinates.second)
				{
					right = true;
				}
				else if ((*item2)->wall_pair.first + 1 == wallpart->coordinates.first && (*item2)->wall_pair.second == wallpart->coordinates.second)
				{
					left = true;
				}
				else if ((*item2)->wall_pair.first == wallpart->coordinates.first && (*item2)->wall_pair.second - 1 == wallpart->coordinates.second)
				{
					down = true;
				}
				else if ((*item2)->wall_pair.first == wallpart->coordinates.first && (*item2)->wall_pair.second + 1 == wallpart->coordinates.second)
				{
					up = true;
				}

				item2++;
			}

			if (down && up)
			{
				//side
				wallpart->wall_anim = side;
			}
			else if (left && right)
			{
				//frontal
				wallpart->wall_anim = front;
			}
			else if (down && right)
			{
				//corner
				wallpart->wall_anim = corner_down_right;
			}
			else if (down && left)
			{
				//corner
				wallpart->wall_anim = corner_down_left;
			}
			else if (up && left)
			{
				//corner
				wallpart->wall_anim = corner_up_left;
			}
			else if (up && right)
			{
				//corner
				wallpart->wall_anim = corner_up_right;
			}
			else if (up || down || left || right)
			{
				//only tower
				wallpart->wall_anim = tower;

			}

			App->entitymanager->wall_parts_list.push_back(wallpart);

			item++;
		}

	}


	return ret;
}

void Walls::LoadAnimations(bool isPlayer1, string path)
{
}