#ifndef __Barracks_H__
#define __Barracks_H__
#include "Building.h"

#define MAX_DEPLOY_SIZE 5

enum class DeployState {
	START,
	DEPLOYING,
	END,
	NONE
};


class Barracks : public Building {
public:
	Barracks();
	Barracks(bool isPlayer1, pair<int, int> position, Collider collider);
	~Barracks();

	bool PreUpdate();
	bool Update(float dt);
	bool DeployTroops(int amount_per_frame = -1);
	void CleanUp();

	void LoadAnimations(bool isPlayer1, string path);

public:

	DeployState		deploy_state = DeployState::END;
	int				deploy_counter = 0;
	pair<int, int>	deploy_pos = { 0,0 };
	bool			is_deploying = false;

	Player* myPlayer;
};

#endif