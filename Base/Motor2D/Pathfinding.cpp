#include "Defs.h"
#include "Log.h"
#include "App.h"
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
	RELEASE_ARRAY(map);
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

void Pathfinding::ChangeWalkability(const pair<int, int>& pos, char cell_type) const
{
	if (cell_type != NULL) 
		map[(pos.second*width) + pos.first] = cell_type;

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