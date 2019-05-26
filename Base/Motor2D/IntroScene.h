#ifndef _INTROSCENE_H_
#define _INTROSCENE_H_

#include "Scene.h"
#include "Gui.h"
#include "Render.h"
#include "Timer.h"
#include "VideoManager.h"

class IntroScene : public Scene
{
public:
	IntroScene();
	~IntroScene();

	bool Start();
	bool Update(float dt);
	bool CleanUp();

private:
	pair<int, int>       win_size;

	bool		 logo_played = false;

	pair<int, int>       UI_pos;
	SDL_Rect UI_image = { 0,0,0,0 };
	UI_Element* UI_window;

	/*UI_Image*	 skip = nullptr;
	UI_Window*	 window = nullptr;*/
};


#endif // !_LOGO_H_
