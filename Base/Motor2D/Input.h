#ifndef __Input_H__
#define __Input_H__

#include "Module.h"
#include "SDL\include\SDL_gamecontroller.h"

//#define NUM_KEYS 352
#define NUM_MOUSE_BUTTONS 5
#define MAX_BUTTONS 13
//#define LAST_KEYS_PRESSED_BUFFER 50

struct SDL_Rect;

enum j1EventWindow
{
	WE_QUIT = 0,
	WE_HIDE = 1,
	WE_SHOW = 2,
	WE_COUNT
};

enum j1KeyState
{
	KEY_IDLE = 0,
	KEY_DOWN,
	KEY_REPEAT,
	KEY_UP
};

enum CONTROLLER_BUTTONS {
	UP = 0,
	DOWN,
	LEFT,
	RIGHT,
	START,
	BACK,
	BUTTON_A,
	BUTTON_B,
	BUTTON_X,
	BUTTON_Y,
	JOY_UP,
	JOY_DOWN,
	JOY_LEFT,
	JOY_RIGHT
};

struct GamePad
{
	SDL_GameController* GameController = nullptr;
	SDL_Joystick* joy = nullptr;

	bool Connected = false;
	bool State[MAX_BUTTONS];

	j1KeyState Controller[MAX_BUTTONS]; //This is the one you check. EXAMPLE: App->input->name.Controller[JOY_RIGHT] == KEY_REPEAT 

	int LeftAxisX = 0;
	int LeftAxisY = 0;

};


class Input : public Module
{

public:

	Input();

	// Destructor
	virtual ~Input();

	// Called before render is available
	bool Awake(pugi::xml_node&);

	// Called before the first frame
	bool Start();

	// Called each loop iteration
	bool PreUpdate();

	// Called before quitting
	bool CleanUp();

	// Gather relevant win events
	bool GetWindowEvent(j1EventWindow ev);

	// Check key states (includes mouse and joy buttons)
	j1KeyState GetKey(int id) const
	{
		return keyboard[id];
	}

	j1KeyState GetMouseButtonDown(int id) const
	{
		return mouse_buttons[id - 1];
	}

	// Check if a certain window event happened
	bool GetWindowEvent(int code);

	// Get mouse / axis position
	void GetMousePosition(int &x, int &y);
	void GetMouseMotion(int& x, int& y);

private:
	bool		windowEvents[WE_COUNT];
	j1KeyState*	keyboard;
	j1KeyState	mouse_buttons[NUM_MOUSE_BUTTONS];
	int			mouse_motion_x;
	int			mouse_motion_y;
	int			mouse_x;
	int			mouse_y;

public:

	GamePad P1; //Player 1 Controller
	GamePad P2; //Player 2 Controller

};

#endif // __Input_H__
