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
	SDL_Texture* texture = nullptr;
	int x = 0;
	int y = 0;
	SDL_Rect Tile_rect;

};

struct decoration_coordinates
{
	pair<int, int> position; 
	string name; 
};

struct Properties
{
	struct Property
	{
		string name;
		int value = 0;
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
	uint		width = 0;
	uint		height = 0;
	uint*		data = nullptr;
	Properties	properties;
	float		parallaxSpeed = 0.0f;

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
	inline void Set(int x, int y, int id) const
	{
		data[(y*width) + x] = id;
	}
};

struct ObjectsData
{
	string		name;	
	string		type;
	int			x = 0;
	int			y = 0;
	int			width = 0;
	int			height = 0;

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
	int					firstgid = 0;
	int					margin = 0;
	int					spacing = 0;
	int					tile_width = 0;
	int					tile_height = 0;
	SDL_Texture*		texture = nullptr;
	int					tex_width = 0;
	int					tex_height = 0;
	int					num_tiles_width = 0;
	int					num_tiles_height = 0;
	int					offset_x = 0; 
	int					offset_y = 0;
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
	int						width = 0;
	int						height = 0;
	int						tile_width = 0;
	int						tile_height = 0;
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

	list<decoration_coordinates*> decoration_list;
	list<pair<int,int>> wall_list;
	list<pair<int,int>> wall_list2;
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
	pair<int, int> MapToWorld(pair<int, int> p) const;
	pair<int,int> WorldToMap(int x, int y) const;
	pair<int, int> WorldToMap(pair<int, int> p) const;


	bool CreateWalkabilityMap(int& width, int& height, uchar** buffer) const;

	//load visble map on list
	void LoadTileList();

	//Draw wakability
	void DrawWalkability(float dt);

	void DrawDecorations(float dt);
	Animation* allied_flag_anim = nullptr;
	Animation* soviet_flag_anim = nullptr;
	Animation* explosion_anim = nullptr;

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
	SDL_Texture*		debug_tex = nullptr;

	MapLayer*			walkability_layer;

	SDL_Texture* imagemap;
	SDL_Rect rect_for_back_image = { 0,0,0,0 };

private:
	pugi::xml_document	map_file;
	string				folder;
	bool				map_loaded = false;

};

#endif // __Map_H__
