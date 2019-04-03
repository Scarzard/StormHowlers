#ifndef __Player_H__
#define __Player_H__

#include "Module.h"
#include "Entity.h"

struct SDL_Texture;
struct ObjectsData;

class Player : public Entity
{
public:

	Player();

	// Destructor
	~Player();

	// Called before render is available
	bool Awake(pugi::xml_node& config);

	//Called before the first frame
	bool Start();

	bool Update(float dt);
	bool PostUpdate();

	// Called each loop iteration

	void Load(pugi::xml_node& file);
	void Save(pugi::xml_node& file) const;

	// Called before quitting
	void CleanUp();

public: 
};

#endif // !__Player_H__
