#ifndef __Pathfinding_H__
#define __Pathfinding_H__

#include "Module.h"

#define DEFAULT_PATH_LENGTH 50
#define INVALID_WALK_CODE 138

// --------------------------------------------------
// Recommended reading:
// Intro: http://www.raywenderlich.com/4946/introduction-to-a-pathfinding
// Details: http://theory.stanford.edu/~amitp/GameProgramming/
// --------------------------------------------------

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

	// Main function to request a path from A to B
	int CreatePath(const pair<int,int>& origin, const pair<int,int>& destination, vector<pair<int,int>>& path_to_fill);

	// To request all tiles involved in the last generated path
	const vector<pair<int,int>>* GetLastPath() const;

	void ResetPath(vector<pair<int,int>>& path_to_reset);

	// Utility: return true if pos is inside the map boundaries
	bool CheckBoundaries(const pair<int,int>& pos) const;

	bool IsTouchingGround(pair<int,int> coords) const;

	// Utility: returns true is the tile is walkable
	bool IsWalkable(const pair<int,int>& pos) const;

	// Utility: return the walkability value of a tile
	uchar GetTileAt(const pair<int,int>& pos) const;

	void ChangeWalkability(const pair<int, int>& pos, bool isWalkable) const;

private:

	// size of the map
	uint width;
	uint height;
	// all map walkability values [0..255]
	uchar* map;
	// we store the created path here
	vector<pair<int,int>> last_path;

};

// forward declaration
struct PathList;

// ---------------------------------------------------------------------
// Pathnode: Helper struct to represent a node in the path creation
// ---------------------------------------------------------------------
struct PathNode
{
	// Convenient constructors
	PathNode();
	PathNode(int g, int h, const pair<int,int>& pos, const PathNode* parent);
	PathNode(const PathNode& node);

	// Fills a list (PathList) of all valid adjacent pathnodes
	uint FindWalkableAdjacents(PathList& list_to_fill) const;
	// Calculates this tile score
	int Score() const;
	// Calculate the F for a specific destination tile
	int CalculateF(const pair<int,int>& destination);

	// -----------
	int g;
	int h;
	pair<int,int> pos;
	const PathNode* parent; // needed to reconstruct the path in the end
};

// ---------------------------------------------------------------------
// Helper struct to include a list of path nodes
// ---------------------------------------------------------------------
struct PathList
{
	// Looks for a node in this list and returns it's list node or NULL
	const PathNode* Find(const pair<int,int>& point) const;

	// Returns the Pathnode with lowest score in this list or NULL if empty
	const PathNode* GetNodeLowestScore() const;

	// -----------
	// The list itself, note they are not pointers!
	list<PathNode> list;
};



#endif // __Pathfinding_H__
