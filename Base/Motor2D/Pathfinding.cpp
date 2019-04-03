#include "Defs.h"
#include "Log.h"
#include "App.h"
#include "Pathfinding.h"

#include "Brofiler\Brofiler.h"

Pathfinding::Pathfinding() : Module(), map(NULL), last_path(DEFAULT_PATH_LENGTH), width(0), height(0)
{
	name.assign("pathfinding");
}

// Destructor
Pathfinding::~Pathfinding()
{
}

// Called before quitting
bool Pathfinding::CleanUp()
{
	BROFILER_CATEGORY("Pathfinding CleanUp", Profiler::Color::Tomato)

	LOG("Freeing pathfinding library");

	last_path.clear();
	RELEASE_ARRAY(map);
	return true;
}

// Sets up the walkability map
void Pathfinding::SetMap(uint width, uint height, uchar* data)
{
	BROFILER_CATEGORY("Pathfinding SetMap", Profiler::Color::Tomato)

	this->width = width;
	this->height = height;

	RELEASE_ARRAY(map);
	map = new uchar[width*height];
	memcpy(map, data, width*height);
}

// Utility: return true if pos is inside the map boundaries
bool Pathfinding::CheckBoundaries(const pair<int,int>& pos) const
{
	BROFILER_CATEGORY("Pathfinding CheckBoundaries", Profiler::Color::Tomato)

	return (pos.first >= 0 && pos.first <= (int)width &&
		pos.second >= 0 && pos.second <= (int)height);
}

bool Pathfinding::IsTouchingGround(pair<int,int> coords) const
{
	return !IsWalkable({ coords.first, coords.second + 1 });
}

// Utility: returns true is the tile is walkable
bool Pathfinding::IsWalkable(const pair<int,int>& pos) const
{
	BROFILER_CATEGORY("Pathfinding IsWalkable", Profiler::Color::Tomato)

	uchar t = GetTileAt(pos);
	return t != INVALID_WALK_CODE && t > 0;
}

// Utility: return the walkability value of a tile
uchar Pathfinding::GetTileAt(const pair<int,int>& pos) const
{
	BROFILER_CATEGORY("Pathfinding GetTileAt", Profiler::Color::Tomato)

	if (CheckBoundaries(pos))
		return map[(pos.second*width) + pos.first];

	return INVALID_WALK_CODE;
}

// To request all tiles involved in the last generated path
const vector<pair<int,int>>* Pathfinding::GetLastPath() const
{
	return &last_path;
}

void Pathfinding::ResetPath(vector<pair<int,int>>& path_to_reset)
{
	path_to_reset.clear();
	last_path.clear();
}

void Pathfinding::ChangeWalkability(const pair<int, int>& pos, bool isWalkable) const
{
	if (isWalkable == true)
		map[(pos.second*width) + pos.first] = 1;
	else
		map[(pos.second*width) + pos.first] = 0;
}


// PathList ------------------------------------------------------------------------
// Looks for a node in this list and returns it's list node or NULL
// ---------------------------------------------------------------------------------
const PathNode* PathList::Find(const pair<int,int>& point) const
{
	BROFILER_CATEGORY("Pathfinding Find", Profiler::Color::Tomato)

	std::list<PathNode>::const_iterator item = list.begin();
	while (item != list.end())
	{
		if ((*item).pos == point)
			return &(*item);
		item++;
	}
	return NULL;
}

// PathList ------------------------------------------------------------------------
// Returns the Pathnode with lowest score in this list or NULL if empty
// ---------------------------------------------------------------------------------
const PathNode* PathList::GetNodeLowestScore() const
{
	BROFILER_CATEGORY("Pathfinding GetNodeLowestScore", Profiler::Color::Tomato)

	const PathNode* ret = NULL;
	int min = 65535;

	std::list <PathNode>::const_reverse_iterator item = list.rbegin();
	while (item != list.rend())
	{
		if ((*item).Score() < min)
		{
			min = (*item).Score();
			ret = &(*item);
		}
		item++;
	}

	return ret;
}

// PathNode -------------------------------------------------------------------------
// Convenient constructors
// ----------------------------------------------------------------------------------
PathNode::PathNode() : g(-1), h(-1), pos(-1, -1), parent(NULL)
{}

PathNode::PathNode(int g, int h, const pair<int,int>& pos, const PathNode* parent) : g(g), h(h), pos(pos), parent(parent)
{}

PathNode::PathNode(const PathNode& node) : g(node.g), h(node.h), pos(node.pos), parent(node.parent)
{}

// PathNode -------------------------------------------------------------------------
// Fills a list (PathList) of all valid adjacent pathnodes
// ----------------------------------------------------------------------------------
uint PathNode::FindWalkableAdjacents(PathList& list_to_fill) const
{
	BROFILER_CATEGORY("Pathfinding FindWalkableAdjacents", Profiler::Color::Tomato)

	pair<int,int> cell;
	uint before = list_to_fill.list.size();

	//// north
	//cell.create(pos.first, pos.second + 1);
	//if (App->pathfinding->IsWalkable(cell))
	//	list_to_fill.list.push_back(PathNode(-1, -1, cell, this));

	//// south
	//cell.create(pos.x, pos.y - 1);
	//if (App->pathfinding->IsWalkable(cell))
	//	list_to_fill.list.push_back(PathNode(-1, -1, cell, this));

	//// east
	//cell.create(pos.x + 1, pos.y);
	//if (App->pathfinding->IsWalkable(cell))
	//	list_to_fill.list.push_back(PathNode(-1, -1, cell, this));

	//// west
	//cell.create(pos.x - 1, pos.y);
	//if (App->pathfinding->IsWalkable(cell))
	//	list_to_fill.list.push_back(PathNode(-1, -1, cell, this));

	return list_to_fill.list.size();
}

// PathNode -------------------------------------------------------------------------
// Calculates this tile score
// ----------------------------------------------------------------------------------
int PathNode::Score() const
{
	return g + h;
}

// PathNode -------------------------------------------------------------------------
// Calculate the F for a specific destination tile
// ----------------------------------------------------------------------------------
int PathNode::CalculateF(const pair<int,int>& destination)
{
	g = parent->g + 1;
	//h = pos.DistanceTo(destination);

	return g + h;
}

// ----------------------------------------------------------------------------------
// Actual A* algorithm: return number of steps in the creation of the path or -1 ----
// ----------------------------------------------------------------------------------
int Pathfinding::CreatePath(const pair<int,int>& origin, const pair<int,int>& destination, vector<pair<int,int>>& path_to_fill)
{
	BROFILER_CATEGORY("Pathfinding CreatePath", Profiler::Color::Tomato)

	// if origin or destination are not walkable, return -1
	if (IsWalkable(origin) == false || IsWalkable(destination) == false) {
		LOG("Not possible to create Path");
		return -1;
	}

	ResetPath(path_to_fill);

	// Create two lists: open, close
	// Add the origin tile to open
	// Iterate while we have tile in the open list
	PathList open;
	PathList close;

	last_path.clear();

	PathNode origin_node;

	origin_node.pos = origin;

	open.list.push_back(origin_node);

	while (open.list.size() > NULL)
	{
		LOG("Creating path %d", last_path.size());

		// Move the lowest score cell from open list to the closed list

		//p2List_item<PathNode>* current_node = open.GetNodeLowestScore();
		//p2List_item<PathNode>* lowest_score_node = close.list.add(current_node->data);

		// If we just added the destination, we are done!
		// Backtrack to create the final path
		// Use the Pathnode::parent and Flip() the path when you are finish

		//if (lowest_score_node->data.pos == destination)
		//{
		//	int steps = 0;
		//	while (lowest_score_node->data.parent != nullptr)
		//	{
		//		last_path.PushBack(lowest_score_node->data.pos);
		//		steps++;
		//		lowest_score_node = close.Find(lowest_score_node->data.parent->pos);
		//	}
		//	last_path.PushBack(lowest_score_node->data.pos);
		//	steps++;

		//	last_path.Flip();
		//	path_to_fill = *GetLastPath();
		//	return steps;
		//}

		// Fill a list of all adjancent nodes
		PathList neighbours;
		//lowest_score_node->data.FindWalkableAdjacents(neighbours);

		// Iterate adjancent nodes:
		// ignore nodes in the closed list
		// If it is NOT found, calculate its F and add it to the open list
		// If it is already in the open list, check if it is a better path (compare G)
		// If it is a better path, Update the parent

		//for (p2List_item<PathNode>* neighbour_node = neighbours.list.start; neighbour_node != nullptr; neighbour_node = neighbour_node->next)
		//{
		//	pair<int,int> neighbour_pos = neighbour_node->data.pos;
		//	if (close.Find(neighbour_pos) != nullptr)
		//		continue;

		//	p2List_item<PathNode>* open_node = open.Find(neighbour_pos);
		//	if (open_node == nullptr)
		//	{
		//		neighbour_node->data.CalculateF(destination);
		//		open.list.add(neighbour_node->data);
		//	}
		//	else
		//	{
		//		int new_g = neighbour_node->data.parent->g + 1;
		//		if (new_g < open_node->data.g)
		//		{
		//			open_node->data.g = new_g;
		//			open_node->data.parent = &lowest_score_node->data;
		//		}
		//	}
		//}
		//open.list.del(current_node);
	}


	return -1;
}

