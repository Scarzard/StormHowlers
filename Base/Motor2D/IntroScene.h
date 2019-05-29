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
	bool			finished = false;
	

};


#endif // !_LOGO_H_
