#include "Defs.h"
#include "Log.h"
#include "App.h"
#include "EntityManager.h"
#include "Pathfinding.h"


Pathfinding::Pathfinding() : Module(), map(NULL), width(0), height(0)
{
	name.assign("Pathfinding");

}

// Destructor
Pathfinding::~Pathfinding()
{
	RELEASE_ARRAY(map);
}

// Called before quitting
bool Pathfinding::CleanUp()
{
	LOG("Freeing Pathfinding library");

	last_path.clear();

	// TODO

	/*for (int i = 0; i < dirMap_p1.size(); i++) {
		RELEASE_ARRAY(dirMap_p1[i]);
	}*/
	dirMap_p1.clear();

	/*for (int i = 0; i < dirMap_p2.size(); i++) {
		RELEASE_ARRAY(dirMap_p2[i]);
	}*/
	dirMap_p2.clear();
	return true;
}

// Sets up the walkability map
void Pathfinding::SetMap(uint width, uint height, uchar* data)
{
	this->width = width;
	this->height = height;

	RELEASE_ARRAY(map);
	map = new uchar[width*height];
	memcpy(map, data, width*height);

	SetDirMap(width,height);
}
bool Pathfinding::Start() {

	debug_tex = App->tex->Load("maps/directions.png");

	map_info = App->gui->AddUIElement(true, UI_Element::LABEL, UI_Element::Action::NONE, { 700,50 }, { 5,5 },nullptr,true);
	map_info->color = { 255,0,0,255 };
	SetInfoLabel();

	//0-5 p1 attack / 5-10 p1 deffense / 10-15 p2 attack / 15-20 p2 deffense
	


	return true;
}

void Pathfinding::SetInfoLabel() {
	string s = (draw_p1_map) ? "P1" : "P2";

	string q = (draw_attack) ? "Attack" : "Defense";

	string t = App->entitymanager->GetName((Entity::entityType)draw_type);

	sprintf_s(label_info, 32, "%s %s %s", s.c_str(), t.c_str(), q.c_str());

	map_info->label = label_info;
}
bool Pathfinding::Update(float dt)
{

	if (App->input->GetKey(SDL_SCANCODE_F1) == KEY_DOWN) {
		draw_p1_map = !draw_p1_map;
		SetInfoLabel();
	}

	if (App->input->GetKey(SDL_SCANCODE_F2) == KEY_DOWN) {
		draw_type = (draw_type % 5) + 8;
		SetInfoLabel();

	}
	if (App->input->GetKey(SDL_SCANCODE_F3) == KEY_DOWN) {
		draw_attack = !draw_attack;
		SetInfoLabel();

	}
	if (App->input->GetKey(SDL_SCANCODE_F5) == KEY_DOWN) {
		draw_dir_map = !draw_dir_map;
		//SetInfoLabel();

	}
	//testing
	if (App->input->GetKey(SDL_SCANCODE_F4) == KEY_DOWN) {
		for (int i = 16; i < 64; i++) {
			for (int j = 0; j < App->map->data.height; j++) {
				SetDirectionAttack(TroopDir::SOUTHWEST, (Entity::entityType::TANKMAN), true, { i, j });
			}
		}
	}
	return true;
}
// Setters MapDir
void Pathfinding::SetDirMap(uint width, uint height) {

	/*RELEASE_ARRAY(dirMap);
	dirMap = new CellInfo[width*height]();*/

	for (int i = 0; i < dirMap_p1.size(); i++) {
		RELEASE_ARRAY(dirMap_p1[i]);
	}
	dirMap_p1.clear();

	for (int i = 0; i < dirMap_p2.size(); i++) {
		RELEASE_ARRAY(dirMap_p2[i]);
	}
	dirMap_p2.clear();

	dirMap_p1 = vector<CellInfo*>(10);
	dirMap_p2 = vector<CellInfo*>(10);

	for (int i = 0; i < 10; i++) {
		dirMap_p1[i] = new CellInfo[width*height]();
		dirMap_p2[i] = new CellInfo[width*height]();
	}


	debug_rects[TroopDir::NORTH] = { 0,0	,60,29};
	debug_rects[TroopDir::SOUTH] = { 120,0	,60,29};
	debug_rects[TroopDir::WEST] = { 60,0	,60,29};
	debug_rects[TroopDir::EAST] = { 180,0	,60,29};
	debug_rects[TroopDir::NORTHEAST]		 = { 60,29	,60,29};
	debug_rects[TroopDir::NORTHWEST]	 = { 120,29, 60,29};
	debug_rects[TroopDir::SOUTHEAST]	 = { 180,29, 60,29};
	debug_rects[TroopDir::SOUTHWEST]		 = { 0,29	,60,29};

	speeds[TroopDir::NORTH] = { 1,-1 };
	speeds[TroopDir::SOUTH] = { -1,1 };
	speeds[TroopDir::WEST] = { -1,-1 };
	speeds[TroopDir::EAST] = { 1,1 };
	speeds[TroopDir::NORTHEAST] = { 1,0 };
	speeds[TroopDir::NORTHWEST] = { 0,-1 };
	speeds[TroopDir::SOUTHEAST] = { 0,1 };
	speeds[TroopDir::SOUTHWEST] = { -1,0 };

	// Setting the map up
	for (int k = Entity::entityType::SOLDIER; k <= Entity::entityType::WAR_HOUND; k++) {
		//Filling borders to keep troops inside
		for (int i = 16; i < 20; i++) {
			for (int j = 0; j < App->map->data.height; j++) {

				SetDirectionAttack (TroopDir::EAST,  (Entity::entityType)k, true, { i, j });
				SetDirectionAttack (TroopDir::EAST,  (Entity::entityType)k, false,{ i, j });
				SetDirectionDefense(TroopDir::EAST, (Entity::entityType)k, true, { i, j });
				SetDirectionDefense(TroopDir::EAST, (Entity::entityType)k, false, { i, j });
			}
		}
		for (int i = 60; i < 64; i++) {
			for (int j = 0; j < App->map->data.height; j++) {

				SetDirectionAttack (TroopDir::WEST, (Entity::entityType)k, true, { i, j });
				SetDirectionAttack(TroopDir::WEST, (Entity::entityType)k, false, { i, j });
				SetDirectionDefense(TroopDir::WEST, (Entity::entityType)k, true, { i, j });
				SetDirectionDefense(TroopDir::WEST, (Entity::entityType)k, false, { i, j });
			}
		}

		int dir;
		// Filling the rest of the map
		for (int i = 20; i < 60; i++) {
			for (int j = 10; j < App->map->data.height-9; j++) {
			
				if ((Entity::entityType)k == Entity::entityType::SOLDIER) {
					SetDirectionAttack(SOUTH, (Entity::entityType)k, true, { i, j });
					SetDirectionAttack (NORTH, (Entity::entityType)k, false, { i, j });
					//if (j > 30) continue;
					dir = rand() % MAX_DIR;
					SetDirectionDefense((TroopDir)dir, (Entity::entityType)k, true, { i, j });
					SetDirectionDefense((TroopDir)dir, (Entity::entityType)k, false, { i, j });
				}
				else {
					SetDirectionAttack(TroopDir::SOUTH, (Entity::entityType)k, true, { i, j });
					SetDirectionAttack(TroopDir::NORTH, (Entity::entityType)k, false, { i, j });
				}
			}
			//TODO set proper limits
			SetDirectionDefense(TroopDir::NORTH, (Entity::entityType)k, true, { i,30});
			SetDirectionDefense(TroopDir::SOUTH, (Entity::entityType)k, false, {i, App->map->warzone.down_limit.first});
		}
		
	}

	

}

//void Pathfinding::UpdateMaps() {
//
//}
void Pathfinding::SetDirectionAttack(TroopDir direction, Entity::entityType type,bool fromPlayer1, pair<int,int> pos) {

	type = (Entity::entityType)((int)type - (int)Entity::entityType::SOLDIER);
	CellInfo* cell = (fromPlayer1) ? &dirMap_p1[type][(pos.second*width) + pos.first]: &dirMap_p2[type][(pos.second*width) + pos.first];
	cell->previousDir.push_back(direction);
	cell->dir = direction;
	cell->speed = speeds[direction];

}
void Pathfinding::SetDirectionDefense(TroopDir direction, Entity::entityType type, bool fromPlayer1, pair<int, int> pos) {

	type = (Entity::entityType)((int)type - (int)Entity::entityType::SOLDIER +5);
	CellInfo* cell = (fromPlayer1) ? &dirMap_p1[type][(pos.second*width) + pos.first] : &dirMap_p2[type][(pos.second*width) + pos.first];
	cell->previousDir.push_back(direction);
	cell->dir = direction;
	cell->speed = speeds[direction];

}

//Getters MapDir

TroopDir Pathfinding::GetDirAttack(int x, int y, Entity::entityType type, bool fromPlayer1) {
	type = (Entity::entityType)((int)type - (int)Entity::entityType::SOLDIER);
	return (fromPlayer1) ? dirMap_p1[type][y*width + x].dir : dirMap_p2[type][y*width + x].dir;
}
TroopDir Pathfinding::GetDirDefense(int x, int y, Entity::entityType type, bool fromPlayer1) {
	type = (Entity::entityType)((int)type - (int)Entity::entityType::SOLDIER); 
	return (fromPlayer1) ? dirMap_p1[type+5][y*width + x].dir : dirMap_p2[type+5][y*width + x].dir;
}

bool Pathfinding::GetHasPathAttack(pair<int, int>pos, Entity::entityType type, bool fromPlayer1) {
	type = (Entity::entityType)((int)type - (int)Entity::entityType::SOLDIER);
	return (fromPlayer1) ? dirMap_p1[type][pos.second*width + pos.first].has_path : dirMap_p2[type][pos.second*width + pos.first].has_path;
}
bool Pathfinding::GetHasPathDefense(pair<int, int>pos, Entity::entityType type, bool fromPlayer1) {
	type = (Entity::entityType)((int)type - (int)Entity::entityType::SOLDIER);
	return (fromPlayer1) ? dirMap_p1[type+5][pos.second*width + pos.first].has_path : dirMap_p2[type+5][pos.second*width + pos.first].has_path;
}

pair<int, int> Pathfinding::GetSpeedAttack(pair<int, int> pos, Entity::entityType type, bool fromPlayer1) {
	type = (Entity::entityType)((int)type - (int)Entity::entityType::SOLDIER);
	return (fromPlayer1) ? dirMap_p1[type][pos.second*width + pos.first].speed : dirMap_p2[type][pos.second*width + pos.first].speed;
}
pair<int, int> Pathfinding::GetSpeedDefense(pair<int, int> pos, Entity::entityType type, bool fromPlayer1) {
	type = (Entity::entityType)((int)type - (int)Entity::entityType::SOLDIER);
	return (fromPlayer1) ? dirMap_p1[type+5][pos.second*width + pos.first].speed : dirMap_p2[type+5][pos.second*width + pos.first].speed;
}


CellInfo* Pathfinding::GetCellAttack(pair<int, int> pos, Entity::entityType type, bool fromPlayer1) {
	type = (Entity::entityType)((int)type - (int)Entity::entityType::SOLDIER);
	return (fromPlayer1) ? &dirMap_p1[type][pos.second*width + pos.first] : &dirMap_p2[type][pos.second*width + pos.first];
}
CellInfo* Pathfinding::GetCellDefense(pair<int, int> pos, Entity::entityType type, bool fromPlayer1) {
	type = (Entity::entityType)((int)type - (int)Entity::entityType::SOLDIER);
	return (fromPlayer1) ? &dirMap_p1[type+5][pos.second*width + pos.first] : &dirMap_p2[type+5][pos.second*width + pos.first];
}
void Pathfinding::DrawDirMap() {

	if (draw_dir_map) {

		if (draw_attack) {

			for (int i = 16; i < 64; i++) {
				for (int j = 10; j < App->map->data.height-9; j++) {
					TroopDir t = GetDirAttack(i, j, (Entity::entityType)draw_type, draw_p1_map);
					if (t == MAX_DIR) continue;
					pair<int,int> pos = App->map->MapToWorld(i, j);
					App->render->Blit(debug_tex, pos.first, pos.second, &debug_rects[t], SDL_FLIP_NONE);
				}
			}
		}
		else {
			for (int i = 16; i < 64; i++) {
				for (int j = 10; j < App->map->data.height-9; j++) {
					TroopDir t = GetDirDefense(i, j, (Entity::entityType)draw_type, draw_p1_map);
					if (t == MAX_DIR) continue;
					pair<int, int> pos = App->map->MapToWorld(i, j);
					App->render->Blit(debug_tex, pos.first, pos.second, &debug_rects[t], SDL_FLIP_NONE);
				}
			}
		}
	}

}

// Utility: return true if pos is inside the map boundaries
bool Pathfinding::CheckBoundaries(const pair<int,int>& pos) const
{
	return (pos.first >= 0 && pos.first <= (int)width &&
		pos.second >= 0 && pos.second <= (int)height);
}

// Utility: returns true is the tile is walkable
bool Pathfinding::IsWalkable(const pair<int,int>& pos) const
{
	uchar t = GetTileAt(pos);
	return t != INVALID_WALK_CODE && t == WALKABLE;
}

// Utility: return the walkability value of a tile
uchar Pathfinding::GetTileAt(const pair<int,int>& pos) const
{
	if (CheckBoundaries(pos))
		return map[(pos.second*width) + pos.first];

	return INVALID_WALK_CODE;
}

// To request all tiles involved in the last generated path
const std::vector<pair<int,int>>* Pathfinding::GetLastPath() const
{
	return &last_path;
}
std::vector<pair<int, int>>* Pathfinding::GetLastPathNotConst()
{
	return &last_path;
}
void Pathfinding::ResetPath(vector<pair<int, int>>& path_to_reset)
{
	path_to_reset.clear();
	last_path.clear();
}

TroopDir Pathfinding::SpeedToDir(pair<int, int> speed) {
	if (speed.first == 0) {
		if (speed.second > 0) {
			return SOUTH;
		}
		else return NORTH;
	}
	else if (speed.first > 0) {
		if (speed.second == 0) return EAST;
		else if (speed.second > 0) return SOUTHEAST;
		else return NORTHEAST;
	}
	else if (speed.first < 0) {
		if (speed.second == 0) return WEST;
		else if (speed.second > 0) return SOUTHWEST;
		else return NORTHWEST;
	}

	return NORTH;
}

TroopDir Pathfinding::SpeedToDirInverse(pair<int, int> speed) {
	if (speed.first == 0) {
		if (speed.second < 0) {
			return SOUTH;
		}
		else return NORTH;
	}
	else if (speed.first < 0) {
		if (speed.second == 0) return EAST;
		else if (speed.second < 0) return SOUTHEAST;
		else return NORTHEAST;
	}
	else if (speed.first > 0) {
		if (speed.second == 0) return WEST;
		else if (speed.second < 0) return SOUTHWEST;
		else return NORTHWEST;
	}
	
	return NORTH;
}

void Pathfinding::CalculatePathsTo(pair<int, int> dest, char cell_type, Collider collider)
{
	int range = 15;
	CellInfo* cell;
	// Molt guarro pero funca per ara
	if (App->entitymanager->entity_list.size() > 102) {

		for (int i = 0; i < range; i++) {
			for (int j = 0; j < range; j++) {

				pair<int, int> next = { dest.first + i - range / 2,dest.second + j - range / 2 };

				if (next.first < 20 || next.first > 59) continue;

				pair<int, int> speed = { -(next.first - dest.first), -(next.second - dest.second )};
				
				for (int k = Entity::entityType::SOLDIER; k <= Entity::entityType::WAR_HOUND; k++) {
					
					switch (cell_type)
					{
					case P1_BUILDING:
						//if (!GetHasPathAttack(next,(Entity::entityType)k,true)) {
							//Player 1 - Repulsion
							//Player 2 - Attraction
							//SetDirectionAttack(SpeedToDir(speed), (Entity::entityType)k, true, next);
							//SetDirectionDefense(SpeedToDirInverse(speed), (Entity::entityType)k, false, next);
						//}
							if ((Entity::entityType)k == Entity::entityType::ENGINEER) {
								SetDirectionDefense(SpeedToDir(speed), (Entity::entityType)k, true, next);
								SetDirectionAttack(SpeedToDir(speed), (Entity::entityType)k, false, next);

							}
							else {
								SetDirectionDefense(SpeedToDirInverse(speed), (Entity::entityType)k, true, next);
								SetDirectionAttack(SpeedToDir(speed), (Entity::entityType)k, false, next);
							}
						break;
					case P2_BUILDING:
						if ((Entity::entityType)k == Entity::entityType::ENGINEER) {
							SetDirectionDefense(SpeedToDir(speed), (Entity::entityType)k, false, next);
							SetDirectionAttack(SpeedToDir(speed), (Entity::entityType)k, true, next);
						}
						else {
							SetDirectionDefense(SpeedToDirInverse(speed), (Entity::entityType)k, false, next);
							SetDirectionAttack(SpeedToDir(speed), (Entity::entityType)k, true, next);
						}
						break;
					case WALKABLE:
						//if (next distance to dest < range) {
							//Default dir
						// TO TEST
						//Defense should diferentiate location and bring home troops?
						cell = GetCellAttack(next, (Entity::entityType)k, true);
						if (cell->previousDir.empty()) {
							SetDirectionDefense(MAX_DIR,(Entity::entityType)k, true, next);
							SetDirectionDefense(MAX_DIR, (Entity::entityType)k, false, next);
							SetDirectionAttack (SOUTH,(Entity::entityType)k, true, next);
							SetDirectionAttack (NORTH, (Entity::entityType)k, false, next);
							/*cell->previousDir.erase();*/
						}
						else {

						SetDirectionDefense(GetCellAttack(next, (Entity::entityType)k, true)->previousDir.back(), (Entity::entityType)k, true, next);
						SetDirectionDefense(GetCellAttack(next, (Entity::entityType)k, false)->previousDir.back(), (Entity::entityType)k, false, next);
						SetDirectionAttack (GetCellAttack(next, (Entity::entityType)k,  true)->previousDir.back(), (Entity::entityType)k, true, next);
						SetDirectionAttack (GetCellAttack(next, (Entity::entityType)k, false)->previousDir.back(), (Entity::entityType)k, false, next);
						}
						

						//}
						break;

					default:
						break;
					}
				}
				//if (!GetHasPath(next)) {
				//	SetDirection(SpeedToDir(speed), next);

				//}
				////if ((dest.first + i >= dest.first - range / 4 && dest.first - i <= dest.first + range / 2) && (dest.second + j >= dest.second - range / 2 && dest.second + j <= dest.second + range / 2)) {
				////if ((dest.first + i >= dest.first+range/4 && dest.first - i <= dest.first-range/2 ) && (dest.second - j <= dest.second-range/4 && dest.second + j <= dest.second+range/4)) {
				//else if(next.first >= dest.first - range/4 && next.first <= dest.first + range/4 && next.second >= dest.second - range/4 && next.second <= dest.second + range/4){
				//	SetDirection(SpeedToDir(speed), next);
				//	/*TroopDir td = GetDir(next);
				//	while (!GetCell(next)->is_axis) {
				//		next.first--;
				//	}*/
				//	/*q = range / 2;*/
				//}
				//else {
				//	continue;
				//}
				//dest.second += speeds[NORTH].second;
			}
			//dest.first += speeds[NORTH].first;
		}

		// Avoid team buildings
		for (int k = Entity::entityType::SOLDIER; k <= Entity::entityType::WAR_HOUND; k++) {
			pair<int, int> next = dest;

			switch (cell_type)
			{
			case P1_BUILDING:
				//next.first -= collider.dimensions.first;
				if (collider.dimensions.first > 2) {
					next.second -= collider.dimensions.second-1;
					next.first -= collider.dimensions.first - 2;
				}
				else {
					next.second -= collider.dimensions.second;
					next.first -= collider.dimensions.first-1;
				}
				
				//First go WEST, rest go EAST
				SetDirectionAttack(WEST, (Entity::entityType)k, true, next);
				for (int i = 1; i < collider.dimensions.first; i++) {
					next.first++;
					SetDirectionAttack(EAST, (Entity::entityType)k, true, next);

				}
				
				break;
			case P2_BUILDING:
				if (collider.dimensions.first > 2) {
					next.second += 2;
					next.first += -2;
				}
				else {
					next.second += 1;
					next.first += -1;
				}

				//First go WEST, rest go EAST
				SetDirectionAttack(WEST, (Entity::entityType)k, false, next);
				for (int i = 1; i < collider.dimensions.first; i++) {
					next.first++;
					SetDirectionAttack(EAST, (Entity::entityType)k, false, next);

				}
				break;
			case WALKABLE:
				break;
			default:
				break;
			}
		
		}


		/*for (int i = NORTH; i < MAX_DIR; i++) {
			range = 9;
			dest = original;
			dest.first += speeds[i].first * range;
			dest.second += speeds[i].second * range;

			pair<int, int> closest_tile = dest;
			while (range > 0) {
				closest_tile.first -= speeds[i].first;
				closest_tile.second -= speeds[i].second;
				if (IsWalkable(closest_tile)) {

					SetDirection((TroopDir)i, { closest_tile.first,closest_tile.second });
				}
				range--;

			}

			SetDirection((TroopDir)i, { dest.first,dest.second });
		}*/
	}

}
void Pathfinding::ChangeWalkability(const pair<int, int>& pos, char cell_type) const 
{
	if (cell_type != NULL) {
		map[(pos.second*width) + pos.first] = cell_type;
		/*if (cell_type == P2_BUILDING) {
			CalculatePathsTo(pos);
		}*/
	}


}
// PathList ------------------------------------------------------------------------
// Looks for a node in this list and returns it's list node or list.end()
// ---------------------------------------------------------------------------------
std::list<PathNode>::iterator PathList::Find(const pair<int,int>& point)
{
	std::list<PathNode>::iterator item = list.begin();

	while (item != list.end())
	{

		if ((*item).pos == point)
			return item;

		item = next(item);
	}

	return list.end();
}

// PathList ------------------------------------------------------------------------
// Returns the Pathnode with lowest score in this list or NULL if empty
// ---------------------------------------------------------------------------------
std::list<PathNode>::iterator PathList::GetNodeLowestScore()
{
	std::list<PathNode>::iterator ret;
	int min = 65535;

	std::list<PathNode>::iterator item = list.begin();

	while (item != list.end())
	{
		if ((*item).Score() < min)
		{
			min = (*item).Score();
			ret = item;
		}
		item = next(item);
	}
	return ret;
}

// PathNode -------------------------------------------------------------------------
// Convenient constructors
// ----------------------------------------------------------------------------------
PathNode::PathNode() : g(-1), h(-1), pos(-1, -1), parent(NULL)
{}

PathNode::PathNode(int g, int h, const pair<int,int>& pos, PathNode* parent) : g(g), h(h), pos(pos), parent(parent)
{}

PathNode::PathNode(const PathNode& node) : g(node.g), h(node.h), pos(node.pos), parent(node.parent)
{}

// PathNode -------------------------------------------------------------------------
// Fills a list (PathList) of all valid adjacent pathnodes
// ----------------------------------------------------------------------------------
uint PathNode::FindWalkableAdjacents(PathList& list_to_fill)
{
	pair<int,int> cell;

	// north
	cell = { pos.first, pos.second + 1 };
	if (App->pathfinding->IsWalkable(cell))
		list_to_fill.list.push_back(PathNode(-1, -1, cell, this));

	// south
	cell = { pos.first, pos.second - 1 };
	if (App->pathfinding->IsWalkable(cell))
		list_to_fill.list.push_back(PathNode(-1, -1, cell, this));

	// east
	cell = { pos.first + 1, pos.second };
	if (App->pathfinding->IsWalkable(cell))
		list_to_fill.list.push_back(PathNode(-1, -1, cell, this));

	// west
	cell = { pos.first - 1, pos.second };
	if (App->pathfinding->IsWalkable(cell))
		list_to_fill.list.push_back(PathNode(-1, -1, cell, this));

	//DIAGONAL
	// north-east
	cell = { pos.first + 1, pos.second + 1 };
	if (App->pathfinding->IsWalkable(cell))
		list_to_fill.list.push_back(PathNode(-1, -1, cell, this));

	// south-east
	cell = { pos.first + 1, pos.second - 1 };
	if (App->pathfinding->IsWalkable(cell))
		list_to_fill.list.push_back(PathNode(-1, -1, cell, this));

	//north-west
	cell = { pos.first - 1, pos.second + 1 };
	if (App->pathfinding->IsWalkable(cell))
		list_to_fill.list.push_back(PathNode(-1, -1, cell, this));

	//south-west
	cell = { pos.first - 1, pos.second - 1 };
	if (App->pathfinding->IsWalkable(cell))
		list_to_fill.list.push_back(PathNode(-1, -1, cell, this));

	return list_to_fill.list.size();
}

// PathNode -------------------------------------------------------------------------
// Calculates this tile score
// ----------------------------------------------------------------------------------
int PathNode::Score() const
{
	return g + h;
}

int PathNode::DistanceTo(const pair<int, int>& destination) {

	int fx = pos.first - destination.first;
	int fy = pos.second - destination.second;

	return int(sqrtf((fx*fx) + (fy*fy)));
}

// PathNode -------------------------------------------------------------------------
// Calculate the F for a specific destination tile
// ----------------------------------------------------------------------------------
int PathNode::CalculateF(const pair<int,int>& destination)
{
	g = parent->g + 1;
	h = DistanceTo(destination);

	//You can also try:
	//h = pos.DistanceManhattan(destination);
	//h = pos.DiagonalDistance(destination);
	//h = pos.DistanceNoSqrt(destination);

	return g + h;
}

// ----------------------------------------------------------------------------------
// Actual A* algorithm: return number of steps in the creation of the path or -1 ----
// ----------------------------------------------------------------------------------
int Pathfinding::PropagateAStar(const pair<int,int>& origin, const pair<int,int>& destination) {

	int ret = -1;

	PathList open;
	PathList close;

	open.list.push_back(PathNode(0, 0, origin, NULL));

	while (open.list.size() > 0) {

		std::list<PathNode>::iterator aux = open.GetNodeLowestScore();
		close.list.push_back(*aux);

		std::list<PathNode>::iterator lower = prev(close.list.end());
		open.list.erase(aux);

		if ((*lower).pos == destination) {

			last_path.clear();
			const PathNode *new_node = &(*lower);

			while (new_node) {

				last_path.push_back(new_node->pos);
				new_node = new_node->parent;
			}

			std::reverse(last_path.begin(), last_path.end());
			ret = last_path.size();
			break;
		}

		PathList AdjacentNodes;
		AdjacentNodes.list.clear();

		(*lower).FindWalkableAdjacents(AdjacentNodes);
		std::list<PathNode>::iterator it = AdjacentNodes.list.begin();

		for (; it != AdjacentNodes.list.end(); it = next(it)) {

			if (close.Find((*it).pos) != close.list.end())
				continue;

			std::list<PathNode>::iterator adj_node = open.Find((*it).pos);

			if (adj_node == open.list.end()) {

				(*it).CalculateF(destination);
				open.list.push_back(*it);
			}
			else if ((*adj_node).g > (*it).g + 1) {

				(*adj_node).parent = (*it).parent;
				(*adj_node).CalculateF(destination);

			}
		}
	}

	return ret;
}


int Pathfinding::CreatePath(const pair<int,int>& origin, const pair<int,int>& destination, bool JPS_active) {

	int ret = -1;

	if (!IsWalkable(origin)) {

		LOG("Origin not Walkable");
		return -1;
	}
	if (!IsWalkable(destination)) {
		LOG("Destination not Walkable");
		return 0;
	}

	if (JPS_active == false)
		PropagateAStar(origin, destination);
	else
		PropagateJPS(origin, destination);

	//LOG("Path Steps: %i", last_path.size());

	return last_path.size();
}


// ----------------------------------------------------------------------------------
// JPS algorithm: returns number of steps in the creation of the path or -1 ----
// ----------------------------------------------------------------------------------
int Pathfinding::PropagateJPS(const pair<int,int>& origin, const pair<int,int>& destination) {

	int ret = -1;

	PathList open;
	PathList close;

	open.list.push_back(PathNode(0, 0, origin, NULL));

	while (open.list.size() > 0) {

		std::list<PathNode>::iterator aux = open.GetNodeLowestScore();
		close.list.push_back(*aux);

		std::list<PathNode>::iterator lower = prev(close.list.end());
		open.list.erase(aux);

		if ((*lower).pos == destination) {

			last_path.clear();
			const PathNode* new_node = &(*lower);

			while (new_node) {

				last_path.push_back(new_node->pos);
				new_node = new_node->parent;
			}

			std::reverse(last_path.begin(), last_path.end());
			ret = last_path.size();
			break;
		}


		//TODO 1: Only difference with A* in the core behaviour: Instead of filling
		//the Adjacent nodes list with the immediate neighbours, we call the function
		//that must prune them
		PathList AdjacentNodes = (*lower).PruneNeighbours(destination, this);

		std::list<PathNode>::iterator it = AdjacentNodes.list.begin();
		for (; it != AdjacentNodes.list.end(); it = next(it)) {

			if (close.Find((*it).pos) != close.list.end())
				continue;

			std::list<PathNode>::iterator adj_node = open.Find((*it).pos);

			if (adj_node == open.list.end()) {

				(*it).CalculateF(destination);
				open.list.push_back(*it);
			}
			else if ((*adj_node).g > (*it).g + 1) {

				(*adj_node).parent = (*it).parent;
				(*adj_node).CalculateF(destination);

			}
		}
	}

	return ret;
}


PathList PathNode::PruneNeighbours(const pair<int,int>& destination, Pathfinding* PF_Module) {

	PathList ret;

	//TODO 2: Here we do the step that A* does in its core and that we deleted in TODO1
	//Fill the neighbours list with the real or immediate neighbours
	//Then iterate it
	PathList neighbours;
	FindWalkableAdjacents(neighbours);

	std::list<PathNode>::iterator item = neighbours.list.begin();
	for (; item != neighbours.list.end(); item = next(item)) {

		//TODO 3: For each iteration, calculate the direction from current node
		//to its neighbour. You can use CLAMP (defined in p2Defs)
		pair<int,int> direction(CLAMP((*item).pos.first - pos.first, 1, -1), CLAMP((*item).pos.second - pos.second, 1, -1));

		//TODO 4: Make a Jump towards the calculated direction to find the next Jump Point
		//and, if any Jump Point is found, add it to the list that we must return
		PathNode* JumpPoint = PF_Module->Jump(pos, direction, destination, this);

		if (JumpPoint != nullptr)
			ret.list.push_back(*JumpPoint);
	}

	return ret;
}


PathNode* Pathfinding::Jump(pair<int,int> current_position, pair<int,int> direction, const pair<int,int>& destination, PathNode* parent) {

	//Determine next possible Jump Point's Position
	pair<int,int> JumpPoint_pos(current_position.first + direction.first, current_position.second + direction.second);

	//If next point isn't walkable, return nullptr
	if (IsWalkable(JumpPoint_pos) == false)
		return nullptr;

	PathNode *ret_JumpPoint = new PathNode(-1, -1, JumpPoint_pos, parent);

	//If next point is goal, return it
	if (ret_JumpPoint->pos == destination)
		return ret_JumpPoint;


	//TODO 5: Check if there is any possible Jump Point in Straight Directions (horizontal and vertical)
	//If found any, return it. Else, keep jumping in the same direction
	/// Checking Horizontals
	if (direction.first != 0 && direction.second == 0) {

		if (IsWalkable({ current_position.first, current_position.second + 1 }) == false && IsWalkable({ current_position.first + direction.first,  current_position.second + 1 }) == true)
			return ret_JumpPoint;

		else if (IsWalkable({ current_position.first, current_position.second - 1 }) == false && IsWalkable({ current_position.first + direction.first,  current_position.second - 1 }) == true)
			return ret_JumpPoint;

	}
	/// Checking Verticals
	else if (direction.first == 0 && direction.second != 0) {

		if (IsWalkable({ current_position.first + 1, current_position.second }) == false && IsWalkable({ current_position.first + 1, current_position.second + direction.second}) == true)
			return ret_JumpPoint;

		else if (IsWalkable({ current_position.first - 1, current_position.second }) == false && IsWalkable({ current_position.first - 1, current_position.second + direction.second }) == true)
			return ret_JumpPoint;
	}
	//TODO 6: Do the same check than for Straight Lines but now for Diagonals!
	//(Remember prunning rules for diagonals!)
	/// Checking Diagonals
	else if (direction.first != 0 && direction.second != 0) {

		if (IsWalkable({ current_position.first + direction.first,current_position.second }) == false)
			return ret_JumpPoint;
		else if (IsWalkable({ current_position.first,current_position.second + direction.second }) == false)
			return ret_JumpPoint;

		if (Jump(JumpPoint_pos, pair<int,int>(direction.first, 0), destination, parent) != nullptr
			|| Jump(JumpPoint_pos, pair<int,int>(0, direction.second), destination, parent) != nullptr)
			return ret_JumpPoint;
	}

	return Jump(JumpPoint_pos, direction, destination, parent);
}