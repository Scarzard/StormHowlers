#ifndef __Window_H__
#define __Window_H__

#include "Module.h"
#include "Render.h"

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
	void GetWindowSize();

public:
	//The window we'll be rendering to
	SDL_Window* window;

	//The surface contained by the window
	SDL_Surface* screen_surface;

	int		width;
	int		height;
	int		scale;

private:
	string	title;
	bool fullscreen=false;
};

#endif // __Window_H__
