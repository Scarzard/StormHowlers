#ifndef __Window_H__
#define __Window_H__

#include "Module.h"
#include "Render.h"
#include "Defs.h"

struct SDL_Window;
struct SDL_Surface;

class Window : public Module
{
public:

	Window();

	// Destructor
	virtual ~Window();

	// Called before render is available
	bool Awake(pugi::xml_node&);

	// Called before quitting
	bool CleanUp();

	// Changae title
	void SetTitle(const char* new_title);

	// Retrive window size
	void GetWindowSize(uint& width, uint& height) const;

	// Retrieve window scale
	uint GetScale() const;

	//tkae windows sizes

	pair <int,int> ModifyWindow();

public:
	//The window we'll be rendering to
	SDL_Window* window;

	//The surface contained by the window
	SDL_Surface* screen_surface;

	int		width = 0;
	int		height = 0;
	int		scale = 0;
	float	zoom_scale = 0;

private:
	string	title;
	bool fullscreen = false;
};

#endif // __Window_H__
