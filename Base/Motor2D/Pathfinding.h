#ifndef __j1Pathfinding_H__
#define __j1Pathfinding_H__

#include "Module.h"
#include "Troop.h"
#include "Gui.h"
#include "Entity.h"
#include <vector>

#define DEFAULT_PATH_LENGTH 50
#define INVALID_WALK_CODE 255

#define NO_WALKABLE  '0'
#define WALKABLE	 '1'

#define P1_SOLDIER	 '2'
#define P1_TROOP	 '4'
#define P1_BUILDING  '5'

#define P2_SOLDIER	 '3'
#define P2_TROOP	 '6'
#define P2_BUILDING  '7'

struct PathNode;

class CellInfo {

public:
	TroopDir dir;
	TroopDir previousDir;
	//Entity* entity;
	pair<int, int> speed;
	bool is_axis = false;
	bool has_path = false;

	CellInfo() : dir(TroopDir::MAX_DIR), previousDir(TroopDir::MAX_DIR), speed({ 0,0 }),has_path(false) {}
};

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

	bool Start();
	void SetInfoLabel();
	bool Update(float dt);
	void SetDirMap(uint width, uint height);

	void SetDirectionAttack(TroopDir direction, Entity::entityType type, bool fromPlayer1, pair<int, int> pos);

	void SetDirectionDefense(TroopDir direction, Entity::entityType type, bool fromPlayer1, pair<int, int> pos);
	
	TroopDir GetDirDefense(int x, int y, Entity::entityType type, bool fromPlayer1);

	TroopDir GetDirAttack(int x, int y, Entity::entityType type, bool fromPlayer1);

	bool GetHasPathAttack(pair<int, int> pos, Entity::entityType type, bool fromPlayer1);

	bool GetHasPathDefense(pair<int, int> pos, Entity::entityType type, bool fromPlayer1);

	pair<int, int> GetSpeedAttack(pair<int, int> pos, Entity::entityType type, bool fromPlayer1);

	pair<int, int> GetSpeedDefense(pair<int, int> pos, Entity::entityType type, bool fromPlayer1);
	
	CellInfo * GetCellAttack(pair<int, int> pos, Entity::entityType type, bool fromPlayer1);

	CellInfo * GetCellDefense(pair<int, int> pos, Entity::entityType type, bool fromPlayer1);


	void DrawDirMap();

	// Main function to request a path from A to B - Decides (with the bool) if expands A* or JPS
	int CreatePath(const pair<int,int>& origin, const pair<int,int>& destination, bool JPS_active);

	// To request all tiles involved in the last generated path
	const std::vector<pair<int,int>>* GetLastPath() const;

	std::vector<pair<int, int>>* GetLastPathNotConst();

	void ResetPath(vector<pair<int, int>>& path_to_reset);

	TroopDir SpeedToDir(pair<int, int> speed);

	TroopDir SpeedToDirInverse(pair<int, int> speed);

	void CalculatePathsTo(pair<int, int> dest, char cell_type, Collider collider);

	void ChangeWalkability( const pair<int, int>& pos, char isWalkable) const;

	//void ChangeWalkability(const pair<int, int>& pos, bool isWalkable) const;

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
	uint width = 0.0f;
	uint height = 0.0f;
	// all map walkability values [0..255]
	uchar* map = nullptr;
	// we store the created path here
	std::vector<pair<int,int>> last_path;

	//CellInfo* dirMap = nullptr;

	SDL_Texture* debug_tex;

	SDL_Rect debug_rects[TroopDir::MAX_DIR];
	pair<int, int> speeds[TroopDir::MAX_DIR];

	vector<CellInfo*> dirMap_p1;
	vector<CellInfo*> dirMap_p2;

	bool draw_p1_map = true;
	bool draw_attack = true;
	//Entity::entityType draw_type = Entity::entityType::SOLDIER;
	
	int draw_type = 8;

	char label_info[32] = "";
	UI_Element* map_info;
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
	int DistanceTo(const pair<int, int>& destination);
	// Calculate the F for a specific destination tile
	int CalculateF(const pair<int,int>& destination);

	// -----------
	int g = 0;
	int h = 0;
	pair<int,int> pos;
	PathNode* parent = nullptr; // needed to reconstruct the path in the end
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