#ifndef __MiniGameScene_H__
#define __MiniGameScene_H__
#include "Module.h"



class MiniGameScene :	public Module
{
	enum miniGameState {
		PREPARING,
		READY,
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

	bool player1_ready = false;
	bool player2_ready = false;
};

#endif