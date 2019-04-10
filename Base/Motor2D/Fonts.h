#ifndef __Fonts_H__
#define __Fonts_H__

#include "Module.h"
#include "SDL\include\SDL_pixels.h"

#define DEFAULT_FONT "fonts/C&C Red Alert [INET].ttf"
#define DEFAULT_FONT_SIZE 17

struct SDL_Texture;
struct _TTF_Font;

class Fonts : public Module
{
public:

	Fonts();

	// Destructor
	virtual ~Fonts();

	// Called before render is available
	bool Awake(pugi::xml_node&);

	// Called before quitting
	bool CleanUp();

	// Load Font
	_TTF_Font* const Load(const char* path, int size = 12);

	// Create a surface from text
	SDL_Texture* Print(const char* text, SDL_Color color = { 255, 255, 255, 255 }, _TTF_Font* font = NULL);

	bool CalcSize(const char* text, int& width, int& height, _TTF_Font* font = NULL) const;

public:

	list<_TTF_Font*>	fonts;
	_TTF_Font*			default;
	_TTF_Font*			actual_font = nullptr;
	list<_TTF_Font*>::iterator font_iterator;
	unsigned int size = 0;
	int default_size = 0;
	const char* path = nullptr;
};


#endif // __Fonts_H__
