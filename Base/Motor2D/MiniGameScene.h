#ifndef __MiniGameScene_H__
#define __MiniGameScene_H__

#include "Module.h"
#include "Timer.h"
#include "Gui.h"

struct SDL_Texture;


class MiniGameScene :	public Module
{
	enum miniGameState {
		PREPARING,
		START,
		PLAYING,
		END
	};

public:

	MiniGameScene();

	bool Start();

	~MiniGameScene();
	bool Update(float dt);

	void ShowSelectTroopsDialog();

public:

	miniGameState mg_state = PREPARING;
	char seconds_to_start;

	int player1_counter = 0;
	int player2_counter = 0;
	int goal_counter = 10;

	char player1_ct_char[64] = "0";
	char player2_ct_char[64] = "0";

	bool player1_ready = false;
	bool player2_ready = false;

	bool player1_won = false;
	bool player2_won = false;

	Timer timer;

	UI_Element* timer_label;
	UI_Element* player1_label;
	UI_Element* player2_label;


	SDL_Texture* tex;
	//SDL_Rect rect;
};

#endif