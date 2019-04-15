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

Walls::Walls(bool isPlayer1, pair<int, int> pos) : Building(entityType::WALLS, isPlayer1, pos)
{
	
	string path = "animation/" + name + ".tmx";
	LoadAnimations(isPlayer1, path.data());
	LoadWalls();
}

Walls::~Walls()
{
}

bool Walls::LoadWalls()
{
	bool ret = true;

	if (fromPlayer1)
	{
		list<wall_coordinates*>::iterator item = App->map->data.wall_list.begin();
		while (item != App->map->data.wall_list.end())
		{
			wall_parts* wallpart = new wall_parts();

			wallpart->coordinates.first = (*item)->wall_pair.first;
			wallpart->coordinates.second = (*item)->wall_pair.second;
			wallpart->health = health;

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
	else if (fromPlayer1==false)
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

	front = front->LoadAnimation(path.data(), (!isPlayer1) ? "principal_wall_red" : "principal_wall_blue");
	front->loop = false;

	side = side->LoadAnimation(path.data(), (!isPlayer1) ? "side_wall_red" : "side_wall_blue");
	side->loop = false;

	corner_down_left = corner_down_left->LoadAnimation(path.data(), (!isPlayer1) ? "corner_east_red" : "corner_east_blue");
	corner_down_left->loop = false;

	tower = tower->LoadAnimation(path.data(), (!isPlayer1) ? "small_wall_red" : "small_wall_blue");
	tower->loop = false;

	corner_down_right = corner_down_right->LoadAnimation(path.data(), (!isPlayer1) ? "corner_north_red" : "corner_north_blue");
	corner_down_right->loop = false;

	corner_up_left = corner_up_left->LoadAnimation(path.data(), (!isPlayer1) ? "corner_south_red" : "corner_south_blue");
	corner_up_left->loop = false;

	corner_up_right = corner_up_right->LoadAnimation(path.data(), (!isPlayer1) ? "corner_west_red" : "corner_west_blue");
	corner_up_right->loop = false;
}
