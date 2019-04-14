#ifndef __j1Pathfinding_H__
#define __j1Pathfinding_H__

#include "Module.h"
#include <vector>

#define DEFAULT_PATH_LENGTH 50
#define INVALID_WALK_CODE 255

struct PathNode;

class Pathfinding : public Module
{
public:

	Pathfinding();

	// Destructor
	~Pathfinding();

	// Called before quitting
	bool CleanUp();

	// Sets up the walkability map
	void SetMap(uint width, uint height, uchar* data);

	// Main function to request a path from A to B - Decides (with the bool) if expands A* or JPS
	int CreatePath(const pair<int,int>& origin, const pair<int,int>& destination, bool JPS_active);

	// To request all tiles involved in the last generated path
	const std::vector<pair<int,int>>* GetLastPath() const;

	void ResetPath(vector<pair<int, int>>& path_to_reset);

	void ChangeWalkability(const pair<int, int>& pos, bool isWalkable) const;

	// Utility: return true if pos is inside the map boundaries
	bool CheckBoundaries(const pair<int,int>& pos) const;

	// Utility: returns true is the tile is walkable
	bool IsWalkable(const pair<int,int>& pos) const;

	// Utility: return the walkability value of a tile
	uchar GetTileAt(const pair<int,int>& pos) const;

public:

	//Runs A*
	int PropagateAStar(const pair<int,int>& origin, const pair<int,int>& destination);

	//Runs JPS
	int PropagateJPS(const pair<int,int>& origin, const pair<int,int>& destination);

	//Decides next Jump Point based on a direction and tile's walkability
	PathNode* Jump(pair<int,int> current_position, pair<int,int> direction, const pair<int,int>& destination, PathNode* parent);

private:

	// size of the map
	uint width;
	uint height;
	// all map walkability values [0..255]
	uchar* map;
	// we store the created path here
	std::vector<pair<int,int>> last_path;
};


struct PathList;

struct PathNode
{
	// Convenient constructors
	PathNode();
	PathNode(int g, int h, const pair<int,int>& pos, PathNode* parent);
	PathNode(const PathNode& node);

	// Fills a list (PathList) of all valid adjacent pathnodes
	uint FindWalkableAdjacents(PathList& list_to_fill);

	//Builds a list with the only nodes that we want to keep considering the direction and calling Jump() function
	//We need to pass the Pathfinding module because Jump() is in there
	PathList PruneNeighbours(const pair<int,int>& destination, Pathfinding* PF_Module = nullptr);

	// Calculates this tile score
	int Score() const;
	// Calculate the F for a specific destination tile
	int CalculateF(const pair<int,int>& destination);

	// -----------
	int g;
	int h;
	pair<int,int> pos;
	PathNode* parent; // needed to reconstruct the path in the end
};


struct PathList
{
	// Looks for a node in this list and returns it's list node or NULL
	std::list<PathNode>::iterator Find(const pair<int,int>& point);

	// Returns the Pathnode with lowest score in this list or NULL if empty
	std::list<PathNode>::iterator GetNodeLowestScore();

	// -----------
	// The list itself, note they are not pointers!
	std::list<PathNode> list;
};



#endif // __j1Pathfinding_H__