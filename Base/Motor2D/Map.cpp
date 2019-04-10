#include "Defs.h"
#include "Log.h"
#include "App.h"
#include "Render.h"
#include "Textures.h"
#include "Window.h"
#include "Map.h"
#include "Pathfinding.h"
#include "Scene.h"
#include "Brofiler\Brofiler.h"
#include <cmath>


Map::Map() : Module(), map_loaded(false)
{
	name.assign("map");
}

// Destructor
Map::~Map()
{}

// Called before render is available
bool Map::Awake(pugi::xml_node& config)
{
	LOG("Loading Map Parser");
	bool ret = true;

	folder.assign(config.child("folder").child_value());
	debug = false;

	//-----
	idleRight123 = idleRight123->LoadAnimation("animation/buildings.tmx", "barracks");
	idleRight123->speed = 1;
	idleRight123->loop = false;
	//-----

	return ret;
}

void Map::Draw(float dt)
{
	BROFILER_CATEGORY("Map Draw", Profiler::Color::Crimson);

	if (map_loaded == false)
		return;

	list<MapLayer*>::const_iterator lay;
	list<TileSet*>::const_iterator set;
	for (lay = data.layers.begin(); lay != data.layers.end(); ++lay)
	{
		MapLayer* layer = *lay;
		for (set = data.tilesets.begin(); set != data.tilesets.end(); ++set)
		{
			if (layer->properties.Get("Navigation") == 1)
				continue;
			for (int y = 0; y < data.height; ++y)
			{
				for (int x = 0; x < data.width; ++x)
				{
					int tile_id = layer->Get(x, y);
					if (tile_id > 0)
					{
						TileSet* tileset = GetTilesetFromTileId(tile_id);
						SDL_Rect r = tileset->GetTileRect(tile_id);
						pair<int, int> pos = MapToWorld(x, y);
						App->render->Blit(tileset->texture, pos.first, pos.second, &r, SDL_FLIP_NONE);

						if (debug == true)
						{
							pos = WorldToMap(pos.first, pos.second);
							if (App->pathfinding->IsWalkable(pos) == false)
							{
								pos = MapToWorld(pos.first, pos.second);
								r = { 60,0,60,29 };
								App->render->Blit(debug_tex, pos.first, pos.second, &r, SDL_FLIP_NONE);
							}
						}
					}
				}
			}
		}
	}

	//testing animation uncoment to blit example
	App->render->Blit(App->scene->spritesheet123, data.main_building.first, data.main_building.second, &idleRight123->GetCurrentFrame(dt));
	App->render->Blit(App->scene->spritesheet123, data.main_building2.first, data.main_building2.second, &idleRight123->GetCurrentFrame(dt));
	
	App->render->Blit(App->scene->spritesheet123, data.main_tower.first, data.main_tower.second, &idleRight123->GetCurrentFrame(dt));
	App->render->Blit(App->scene->spritesheet123, data.main_tower2.first, data.main_tower2.second, &idleRight123->GetCurrentFrame(dt));

	App->render->Blit(App->scene->spritesheet123, data.special_skill.first, data.special_skill.second, &idleRight123->GetCurrentFrame(dt));
	App->render->Blit(App->scene->spritesheet123, data.special_skill2.first, data.special_skill2.second, &idleRight123->GetCurrentFrame(dt));

	App->render->Blit(App->scene->spritesheet123, data.tower.first, data.tower.second, &idleRight123->GetCurrentFrame(dt));
	App->render->Blit(App->scene->spritesheet123, data.tower2.first, data.tower2.second, &idleRight123->GetCurrentFrame(dt));

	App->render->Blit(App->scene->spritesheet123, data.barrack.first, data.barrack.second, &idleRight123->GetCurrentFrame(dt));
	App->render->Blit(App->scene->spritesheet123, data.barrack2.first, data.barrack2.second, &idleRight123->GetCurrentFrame(dt));

	App->render->Blit(App->scene->spritesheet123, data.mid_building.first, data.mid_building.second, &idleRight123->GetCurrentFrame(dt));
	
}

int Properties::Get(const char* value, int default_value) const
{
	std::list<Property*>::const_iterator item = list.begin();

	while (item != list.end())
	{
		if ((*item)->name == value)
			return (*item)->value;
		item++;
	}

	return default_value;
}

TileSet* Map::GetTilesetFromTileId(int id) const
{
	// Tileset based on a tile id
	list<TileSet*>::const_iterator item = data.tilesets.begin();
	TileSet* set = *item;

	while (item != data.tilesets.end())
	{
		if (id < (*item)->firstgid)
		{
			set = *prev(item, 1);
			break;
		}
		set = (*item);
		item++;
	}

	return set;
}

pair<int, int> Map::MapToWorld(int x, int y) const
{
	pair<int,int> ret;

	if(data.type == MAPTYPE_ORTHOGONAL)
	{
		ret.first = x * data.tile_width;
		ret.second = y * data.tile_height;
	}
	else if(data.type == MAPTYPE_ISOMETRIC)
	{
		ret.first = (x - y) * ceilf(data.tile_width * 0.5f);
		ret.second = (x + y) * ceilf(data.tile_height * 0.5f);
	}
	else
	{
		LOG("Unknown map type");
		ret.first = x; ret.second = y;
	}

	return ret;
}

pair<int,int> Map::WorldToMap(int x, int y) const
{
	pair<int,int> ret(0,0);

	if(data.type == MAPTYPE_ORTHOGONAL)
	{
		ret.first = x / data.tile_width;
		ret.second = y / data.tile_height;
	}
	else if(data.type == MAPTYPE_ISOMETRIC)
	{
		
		float half_width = data.tile_width * 0.5f;
		float half_height = data.tile_height * 0.5f;
		ret.first = int( (x / half_width + y / half_height) / 2);
		ret.second = int( (y / half_height - (x / half_width)) / 2);
	}
	else
	{
		LOG("Unknown map type");
		ret.first = x; ret.second = y;
	}

	return ret;
}

SDL_Rect TileSet::GetTileRect(int id) const
{
	int relative_id = id - firstgid;
	SDL_Rect rect;
	rect.w = tile_width;
	rect.h = tile_height;
	rect.x = margin + ((rect.w + spacing) * (relative_id % num_tiles_width));
	rect.y = margin + ((rect.h + spacing) * (relative_id / num_tiles_width));
	return rect;
}

// Called before quitting
bool Map::CleanUp()
{
	LOG("Unloading map");

	// Remove all tilesets
	list<TileSet*>::iterator item = data.tilesets.begin();
	while(item != data.tilesets.end())
	{
		SDL_DestroyTexture((*item)->texture);
		RELEASE(*item);
		item++;
	}
	data.tilesets.clear();

	//Remove all object layers
	list<ObjectsGroup*>::iterator item1 = data.objLayers.begin();
	while (item1 != data.objLayers.end())
	{
		RELEASE(*item1);
		item1++;
	}
	data.objLayers.clear();

	// Remove all layers
	list<MapLayer*>::iterator item2 = data.layers.begin();
	while(item2 != data.layers.end())
	{
		RELEASE(*item2);
		item2++;
	}
	data.layers.clear();

	// Clean up the pugui tree
	map_file.reset();

	return true;
}

// Load new map
bool Map::Load(const char* file_name)
{
	bool ret = true;

	string tmp;
	tmp.assign(folder.data());
	tmp += file_name;

	pugi::xml_parse_result result = map_file.load_file(tmp.data());

	if(result == NULL)
	{
		LOG("Could not load map xml file %s. pugi error: %s", file_name, result.description());
		ret = false;
	}

	// Load general info ----------------------------------------------
	if(ret == true)
	{
		ret = LoadMap();
	}

	// Load all tilesets info ----------------------------------------------
	pugi::xml_node tileset;
	for(tileset = map_file.child("map").child("tileset"); tileset && ret; tileset = tileset.next_sibling("tileset"))
	{
		TileSet* set = new TileSet();

		if(ret == true)
		{
			ret = LoadTilesetDetails(tileset, set);
		}

		if(ret == true)
		{
			ret = LoadTilesetImage(tileset, set);
		}

		data.tilesets.push_back(set);
	}

	// Load layer info ----------------------------------------------
	pugi::xml_node layer;
	for(layer = map_file.child("map").child("layer"); layer && ret; layer = layer.next_sibling("layer"))
	{
		MapLayer* lay = new MapLayer();

		ret = LoadLayer(layer, lay);

		if(ret == true)
			data.layers.push_back(lay);
	}

	//Load objects info ----------------------------------------------
	pugi::xml_node group;
	for (group = map_file.child("map").child("objectgroup"); group && ret; group = group.next_sibling("objectgroup"))
	{
		ObjectsGroup* set = new ObjectsGroup();

		if (ret == true)
		{
			ret = LoadObjectLayers(group, set);
		}
		data.objLayers.push_back(set);
	}


	if(ret == true)
	{
		LOG("Successfully parsed map XML file: %s", file_name);
		LOG("width: %d height: %d", data.width, data.height);
		LOG("tile_width: %d tile_height: %d", data.tile_width, data.tile_height);

		list<TileSet*>::const_iterator item = data.tilesets.begin();
		while(item != data.tilesets.end())
		{
			TileSet* s = *item;
			LOG("Tileset ----");
			LOG("name: %s firstgid: %d", s->name.data(), s->firstgid);
			LOG("tile width: %d tile height: %d", s->tile_width, s->tile_height);
			LOG("spacing: %d margin: %d", s->spacing, s->margin);
			item++;
		}

		list<MapLayer*>::const_iterator item_layer = data.layers.begin();
		while(item_layer != data.layers.end())
		{
			MapLayer* l = *item_layer;
			LOG("Layer ----");
			LOG("name: %s", l->name.data());
			LOG("tile width: %d tile height: %d", l->width, l->height);
			item_layer++;
		}

		list<ObjectsGroup*>::const_iterator obj_layer = data.objLayers.begin();
		while (obj_layer != data.objLayers.end())
		{
			ObjectsGroup* o = *obj_layer;
			LOG("Group ----");
			LOG("name: %s", o->name.data());

			obj_layer++;
		}

	}

	map_loaded = ret;

	return ret;
}

// Load map general properties
bool Map::LoadMap()
{
	bool ret = true;
	pugi::xml_node map = map_file.child("map");

	if(map == NULL)
	{
		LOG("Error parsing map xml file: Cannot find 'map' tag.");
		ret = false;
	}
	else
	{
		data.width = map.attribute("width").as_int();
		data.height = map.attribute("height").as_int();
		data.tile_width = map.attribute("tilewidth").as_int();
		data.tile_height = map.attribute("tileheight").as_int();
		string bg_color(map.attribute("backgroundcolor").as_string());


		data.background_color.r = 0;
		data.background_color.g = 0;
		data.background_color.b = 0;
		data.background_color.a = 0;

		if(bg_color.size() > 0)
		{
			string red, green, blue;
			bg_color.substr(1, 2);
			bg_color.substr(3, 4);
			bg_color.substr(5, 6);

			int v = 0;

			sscanf_s(red.data(), "%x", &v);
			if(v >= 0 && v <= 255) data.background_color.r = v;

			sscanf_s(green.data(), "%x", &v);
			if(v >= 0 && v <= 255) data.background_color.g = v;

			sscanf_s(blue.data(), "%x", &v);
			if(v >= 0 && v <= 255) data.background_color.b = v;
		}

		string orientation(map.attribute("orientation").as_string());

		if(orientation == "orthogonal")
		{
			data.type = MAPTYPE_ORTHOGONAL;
		}
		else if(orientation == "isometric")
		{
			data.type = MAPTYPE_ISOMETRIC;
		}
		else if(orientation == "staggered")
		{
			data.type = MAPTYPE_STAGGERED;
		}
		else
		{
			data.type = MAPTYPE_UNKNOWN;
		}

		//loading gameobjects
		pugi::xml_node mapIterator;
		for (mapIterator = map.child("objectgroup"); mapIterator; mapIterator = mapIterator.next_sibling("objectgroup"))
		{
			string tmp(mapIterator.first_attribute().as_string());
			if (tmp == "main base")
			{
				data.main_building.first = mapIterator.child("object").attribute("x").as_int();
				data.main_building.second = mapIterator.child("object").attribute("y").as_int();
				//data.main_building = MapToWorld(data.main_building.first, data.main_building.second);
			}
			else if (tmp == "main base 2")
			{
				data.main_building2.first = mapIterator.child("object").attribute("x").as_int();
				data.main_building2.second = mapIterator.child("object").attribute("y").as_int();
			//	data.main_building2 = MapToWorld(data.main_building2.first, data.main_building2.second);
			}
			else if (tmp == "main tower")
			{
				data.main_tower.first = mapIterator.child("object").attribute("x").as_int();
				data.main_tower.second = mapIterator.child("object").attribute("y").as_int();
				//data.main_tower = MapToWorld(data.main_tower.first, data.main_tower.second);
			}
			else if (tmp == "main tower 2")
			{
				data.main_tower2.first = mapIterator.child("object").attribute("x").as_int();
				data.main_tower2.second = mapIterator.child("object").attribute("y").as_int();
				//data.main_tower2 = MapToWorld(data.main_tower2.first, data.main_tower2.second);
			}
			else if (tmp == "special skills")
			{
				data.special_skill.first = mapIterator.child("object").attribute("x").as_int();
				data.special_skill.second = mapIterator.child("object").attribute("y").as_int();
			//	data.special_skill = MapToWorld(data.special_skill.first, data.special_skill.second);
			}
			else if (tmp == "special skills 2")
			{
				data.special_skill2.first = mapIterator.child("object").attribute("x").as_int();
				data.special_skill2.second = mapIterator.child("object").attribute("y").as_int();
				//data.special_skill2 = MapToWorld(data.special_skill2.first, data.special_skill2.second);
			}
			else if (tmp == "tower")
			{
				data.tower.first = mapIterator.child("object").attribute("x").as_int();
				data.tower.second = mapIterator.child("object").attribute("y").as_int();
				//data.tower = MapToWorld(data.tower.first, data.tower.second);
			}
			else if (tmp == "tower 2")
			{
				data.tower2.first = mapIterator.child("object").attribute("x").as_int();
				data.tower2.second = mapIterator.child("object").attribute("y").as_int();
			//data.tower2 = MapToWorld(data.tower2.first, data.tower2.second);
			}
			else if (tmp == "barrack")
			{
				data.barrack.first = mapIterator.child("object").attribute("x").as_int();
				data.barrack.second = mapIterator.child("object").attribute("y").as_int();
			//	data.barrack = MapToWorld(data.barrack.first, data.barrack.second);
			}
			else if (tmp == "barrack 2")
			{
				data.barrack2.first = mapIterator.child("object").attribute("x").as_int();
				data.barrack2.second = mapIterator.child("object").attribute("y").as_int();
			//	data.barrack2 = MapToWorld(data.barrack2.first, data.barrack2.second);
			}
			else if (tmp == "mid building")
			{
				data.mid_building.first = mapIterator.child("object").attribute("x").as_int();
				data.mid_building.second = mapIterator.child("object").attribute("y").as_int();
				//data.mid_building = MapToWorld(data.mid_building.first, data.mid_building.second);
			}

		}
	}

	return ret;
}

bool Map::LoadTilesetDetails(pugi::xml_node& tileset_node, TileSet* set)
{
	bool ret = true;
	set->name.assign(tileset_node.attribute("name").as_string());
	set->firstgid = tileset_node.attribute("firstgid").as_int();
	set->tile_width = tileset_node.attribute("tilewidth").as_int();
	set->tile_height = tileset_node.attribute("tileheight").as_int();
	set->margin = tileset_node.attribute("margin").as_int();
	set->spacing = tileset_node.attribute("spacing").as_int();
	pugi::xml_node offset = tileset_node.child("tileoffset");

	if(offset != NULL)
	{
		set->offset_x = offset.attribute("x").as_int();
		set->offset_y = offset.attribute("y").as_int();
	}
	else
	{
		set->offset_x = 0;
		set->offset_y = 0;
	}

	return ret;
}

bool Map::LoadTilesetImage(pugi::xml_node& tileset_node, TileSet* set)
{
	bool ret = true;
	pugi::xml_node image = tileset_node.child("image");

	if(image == NULL)
	{
		LOG("Error parsing tileset xml file: Cannot find 'image' tag.");
		ret = false;
	}
	else
	{
		set->texture = App->tex->Load(PATH(folder.data(), image.attribute("source").as_string()));
		int w, h;
		SDL_QueryTexture(set->texture, NULL, NULL, &w, &h);
		set->tex_width = image.attribute("width").as_int();

		if(set->tex_width <= 0)
		{
			set->tex_width = w;
		}

		set->tex_height = image.attribute("height").as_int();

		if(set->tex_height <= 0)
		{
			set->tex_height = h;
		}

		set->num_tiles_width = set->tex_width / set->tile_width;
		set->num_tiles_height = set->tex_height / set->tile_height;
	}

	return ret;
}

bool Map::LoadLayer(pugi::xml_node& node, MapLayer* layer)
{
	bool ret = true;

	layer->name = node.attribute("name").as_string();
	layer->width = node.attribute("width").as_uint();
	layer->height = node.attribute("height").as_uint();
	LoadProperties(node, layer->properties);
	layer->data = new uint[layer->width * layer->height];
	layer->parallaxSpeed = node.child("properties").child("property").attribute("value").as_float(0.0f);

	pugi::xml_node layer_data = node.child("data");

	if(layer_data == NULL)
	{
		LOG("Error parsing map xml file: Cannot find 'layer/data' tag.");
		ret = false;
		RELEASE(layer);
	}
	else
	{
		layer->data = new uint[layer->width*layer->height];
		memset(layer->data, 0, layer->width*layer->height);

		int i = 0;
		for(pugi::xml_node tile = layer_data.child("tile"); tile; tile = tile.next_sibling("tile"))
		{
			layer->data[i++] = tile.attribute("gid").as_int(0);
		}
	}

	return ret;
}

bool Map::LoadObjectLayers(pugi::xml_node & node, ObjectsGroup * group)
{
	bool ret = true;

	group->name = node.attribute("name").as_string();

	for (pugi::xml_node& object = node.child("object"); object && ret; object = object.next_sibling("object"))
	{
		ObjectsData* data = new ObjectsData;

		data->height = object.attribute("height").as_uint();
		data->width = object.attribute("width").as_uint();
		data->x = object.attribute("x").as_uint();
		data->y = object.attribute("y").as_uint();
		data->name = object.attribute("name").as_string();
		data->type = object.attribute("type").as_string();

		group->objects.push_back(data);
	}
	return ret;
}

// Load a group of properties from a node and fill a list with it
bool Map::LoadProperties(pugi::xml_node& node, Properties& properties)
{
	bool ret = false;

	pugi::xml_node data = node.child("properties");

	if (data != NULL)
	{
		pugi::xml_node properties_node;

		for (properties_node = data.child("property"); properties_node != NULL; properties_node = properties_node.next_sibling("property"))
		{
			Properties::Property* property = new Properties::Property();

			property->name = properties_node.attribute("name").as_string();
			property->value = properties_node.attribute("value").as_int();

			properties.list.push_back(property);
		}
	}
	return ret;
}

bool Map::SwitchMaps(string* new_map)
{
	CleanUp();
	Load(new_map->data());
	App->scene->to_end = false;

	return true;
}

bool Map::CreateWalkabilityMap(int& width, int& height, uchar** buffer) const
{
	bool ret = false;
	
	list<MapLayer*>::const_iterator item;
	for (item = data.layers.begin(); item != data.layers.end(); item++)
	{
		MapLayer* layer = *item;
		if (layer->properties.Get("Navigation") == 0)
			continue;
		uchar* map = new uchar[layer->width*layer->height];
		memset(map, 1, layer->width*layer->height);
		for (int y = 0; y < data.height; ++y)
		{
			for (int x = 0; x < data.width; ++x)
			{
				int i = (y*layer->width) + x;
				int tile_id = layer->Get(x, y);
				TileSet* tileset = (tile_id > 0) ? GetTilesetFromTileId(tile_id) : NULL;
				if (tileset != NULL)
				{
					map[i] = (tile_id - tileset->firstgid) > 0 ? 0 : 1;
					
				}
			}
		}
		*buffer = map;
		width = data.width;
		height = data.height;
		ret = true;
		break;
	}
	return ret;
}