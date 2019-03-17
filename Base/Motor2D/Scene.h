#ifndef __Scene_H__
#define __Scene_H__

#include "Module.h"
#include "UI_Element.h"

struct SDL_Texture;

class Scene : public Module
{
public:

	Scene();

	// Destructor
	virtual ~Scene();

	// Called before render is available
	bool Awake(pugi::xml_node&);

	// Called before the first frame
	bool Start();

	// Called before all Updates
	bool PreUpdate();

	// Called each loop iteration
	bool Update(float dt);

	// Called before all Updates
	bool PostUpdate();

	// Called before quitting
	bool CleanUp();

	bool Load(pugi::xml_node& data);
	bool Save(pugi::xml_node& data) const;

	bool Load_level(int map);
	void SpawnEnemies();
	void SpawnEntities();

public:

	vector<string*> map_names;
	uint currentMap = 0;
	float fade_time = 0.0f;
	bool to_end = false;
	bool change = false;
	bool pause = false;

	bool godmode = false;

	string current_track;
	string current_fx;

private:

	SDL_Texture* debug_tex;
};

#endif // __Scene_H__
