#ifndef __CursorModule_H__
#define __CursorModule_H__

#include "Module.h"

struct SDL_Texture;

struct Cursor
{
	pair <int, int> position;
	SDL_Rect area;
};

class CursorModule : public Module
{
public:

	CursorModule();
	~CursorModule();

	// Called before render is available
	bool Awake();

	// Called before the first frame
	bool Start();

	// Called each loop iteration
	bool Update(float dt);

	// Called before all Updates
	bool PostUpdate();

	// Called before quitting
	bool CleanUp();

	void GetCursor1_Position(int& x, int& y);
	void GetCursor2_Position(int& x, int& y);

private:

	Cursor cP1, cP2;

	SDL_Texture*	graphics = nullptr;

};

#endif // __CursorModule_H__
