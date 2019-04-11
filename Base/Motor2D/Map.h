#ifndef __Map_H__
#define __Map_H__

#include "PugiXml/src/pugixml.hpp"
#include "Module.h"
#include "SDL\include\SDL_rect.h"
//testing animation
#include "Animation.h"


struct SDL_Texture;
struct Tiles
{
	SDL_Texture* texture;
	int x;
	int y;
	SDL_Rect Tile_rect;

};
struct Properties
{
	struct Property
	{
		string name;
		int value;
	};

	~Properties()
	{
		std::list<Property*>::iterator item = list.begin();

		while (item != list.end())
		{
			RELEASE(*item);
			item++;
		}

		list.clear();
	}

	int Get(const char* name, int default_value = 0) const;

	list<Property*>	list;
};

// ----------------------------------------------------
struct MapLayer
{
	string		name;
	uint		width;
	uint		height;
	uint*		data;
	Properties	properties;
	float		parallaxSpeed;

	MapLayer() : data(NULL)
	{}

	~MapLayer()
	{
		RELEASE_ARRAY(data);
	}

	inline uint Get(int x, int y) const
	{
		return data[(y*width) + x];
	}
};

struct ObjectsData
{
	string		name;	
	string		type;
	int			x;
	int			y;
	int			width;
	int			height;

};

struct ObjectsGroup
{
	string				name;
	list<ObjectsData*>	objects;

	~ObjectsGroup()
	{
		list<ObjectsData*>::iterator item = objects.begin();

		while (item != objects.end())
		{
			RELEASE(*item);
			item++;
		}

		objects.clear();
	}
};


// ----------------------------------------------------
struct TileSet
{
	SDL_Rect GetTileRect(int id) const;

	string				name;
	int					firstgid;
	int					margin;
	int					spacing;
	int					tile_width;
	int					tile_height;
	SDL_Texture*		texture;
	int					tex_width;
	int					tex_height;
	int					num_tiles_width;
	int					num_tiles_height;
	int					offset_x;
	int					offset_y;
};

enum MapTypes
{
	MAPTYPE_UNKNOWN = 0,
	MAPTYPE_ORTHOGONAL,
	MAPTYPE_ISOMETRIC,
	MAPTYPE_STAGGERED
};
// ----------------------------------------------------
struct MapData
{
	int						width;
	int						height;
	int						tile_width;
	int						tile_height;
	SDL_Color				background_color;
	MapTypes				type;
	list<TileSet*>		tilesets;
	list<MapLayer*>		layers;
	list<ObjectsGroup*>	objLayers;

	//entities
	pair<int, int> main_building;
	pair<int, int> main_building2;
	pair<int, int> main_tower;
	pair<int, int> main_tower2;
	pair<int, int> special_skill;
	pair<int, int> special_skill2;
	pair<int, int> tower;
	pair<int, int> tower2;
	pair<int, int> barrack;
	pair<int, int> barrack2;
	pair<int, int> mid_building;
};

// ----------------------------------------------------
class Map : public Module
{
public:

	Map();

	// Destructor
	virtual ~Map();

	// Called before render is available
	bool Awake(pugi::xml_node& conf);

	// Called each loop iteration
	void Draw(float dt);

	// Called before quitting
	bool CleanUp();

	// Load new map
	bool Load(const char* path);

	//Unloads the current map and loads a new one 
	bool SwitchMaps(string* new_map);

	pair<int,int> MapToWorld(int x, int y) const;
	pair<int,int> WorldToMap(int x, int y) const;

	bool CreateWalkabilityMap(int& width, int& height, uchar** buffer) const;

	//load visble map on list
	void LoadTileList();

	//Draw wakability
	void DrawWakability(float dt);

private:

	bool LoadMap();
	bool LoadTilesetDetails(pugi::xml_node& tileset_node, TileSet* set);
	bool LoadTilesetImage(pugi::xml_node& tileset_node, TileSet* set);
	bool LoadLayer(pugi::xml_node& node, MapLayer* layer);
	bool LoadProperties(pugi::xml_node& node, Properties& properties);
	bool LoadObjectLayers(pugi::xml_node& node, ObjectsGroup* group);

	TileSet* GetTilesetFromTileId(int id) const;

	//testing animation
	Animation* idleRight123 = nullptr;
	

	//map framerate
	list<Tiles>		TileList;

public:
	MapData				data;
	bool				debug = false;
	SDL_Texture*		debug_tex;

private:
	pugi::xml_document	map_file;
	string				folder;
	bool				map_loaded;
};

#endif // __Map_H__
