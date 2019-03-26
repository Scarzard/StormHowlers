#ifndef __Player_H__
#define __Player_H__

#include "Module.h"
#include "Entity.h"

struct SDL_Texture;

class Player : public Module
{
public:

	Player();
	~Player();

	bool Awake(pugi::xml_node& config);
	bool Start();
	bool Update(float dt);
	bool PostUpdate();


	// Called before quitting
	void CleanUp();

};

#endif // !__Player_H__