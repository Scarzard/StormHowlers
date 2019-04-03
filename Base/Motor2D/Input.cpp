#include "Defs.h"
#include "Log.h"
#include "App.h"
#include "Input.h"
#include "Window.h"
#include "Brofiler\Brofiler.h"
#include "SDL/include/SDL.h"

#define MAX_KEYS 300
#define DEATH_ZONE 6400

Input::Input() : Module()
{
	name.assign("input");

	keyboard = new j1KeyState[MAX_KEYS];
	memset(keyboard, KEY_IDLE, sizeof(j1KeyState) * MAX_KEYS);
	memset(mouse_buttons, KEY_IDLE, sizeof(j1KeyState) * NUM_MOUSE_BUTTONS);
}

// Destructor
Input::~Input()
{
	delete[] keyboard;
}

// Called before render is available
bool Input::Awake(pugi::xml_node& config)
{
	LOG("Init SDL input event system");
	bool ret = true;
	SDL_Init(0);
	SDL_Init(SDL_INIT_GAMECONTROLLER);

	if(SDL_InitSubSystem(SDL_INIT_EVENTS) < 0)
	{
		LOG("SDL_EVENTS could not initialize! SDL_Error: %s\n", SDL_GetError());
		ret = false;
	}

	if (SDL_NumJoysticks() < 1)
	{
		LOG("Warning: No joysticks connected!\n");
	}
	else
	{
		if (SDL_NumJoysticks() == 2) //Here we have to put instead of a 2 a 1 (im doing it with ps4 controllers and its bugged, with XBOX controllers will work with a 1) 
		{
			//If there is only 1 controller connected
			P1.GameController = SDL_GameControllerOpen(0);
			P1.joy = SDL_JoystickOpen(0);
		}
		else if (SDL_NumJoysticks() == 4) //Here we have to put instead of a 4 a 2 (im doing it with ps4 controllers and its bugged, with XBOX controllers will work with a 2)
		{
			// If there are 2 controllers connected
			P1.GameController = SDL_GameControllerOpen(0);
			P1.joy = SDL_JoystickOpen(0);

			P2.GameController = SDL_GameControllerOpen(1);
			P2.joy = SDL_JoystickOpen(1);
		}


	}

	return ret;
}

// Called before the first frame
bool Input::Start()
{
	SDL_StopTextInput();
	return true;
}

// Called each loop iteration
bool Input::PreUpdate()
{
	BROFILER_CATEGORY("Input PreUpdate", Profiler::Color::GreenYellow);

	static SDL_Event event;
	
	const Uint8* keys = SDL_GetKeyboardState(NULL);

	for(int i = 0; i < MAX_KEYS; ++i)
	{
		if(keys[i] == 1)
		{
			if(keyboard[i] == KEY_IDLE)
				keyboard[i] = KEY_DOWN;
			else
				keyboard[i] = KEY_REPEAT;
		}
		else
		{
			if(keyboard[i] == KEY_REPEAT || keyboard[i] == KEY_DOWN)
				keyboard[i] = KEY_UP;
			else
				keyboard[i] = KEY_IDLE;
		}
	}

	for(int i = 0; i < NUM_MOUSE_BUTTONS; ++i)
	{
		if(mouse_buttons[i] == KEY_DOWN)
			mouse_buttons[i] = KEY_REPEAT;

		if(mouse_buttons[i] == KEY_UP)
			mouse_buttons[i] = KEY_IDLE;
	}

	for (int i = 0; i < SDL_NumJoysticks() / 2; ++i) //Remove the /2 for XBOX controller (with ps4 works weirdly)
	{
		if (SDL_IsGameController(i))
		{
			if (i == 0)
			{
				if (SDL_IsGameController(i))
				{
					P1.GameController = SDL_GameControllerOpen(i);
					P1.joy = SDL_JoystickOpen(i);
					if (SDL_GameControllerGetAttached(P1.GameController))
					{
						// CONTROLLER 1 ----------------------------------------------------------------------------------------
						P1.State[UP] = SDL_GameControllerGetButton(P1.GameController, SDL_CONTROLLER_BUTTON_DPAD_UP);
						P1.State[DOWN] = SDL_GameControllerGetButton(P1.GameController, SDL_CONTROLLER_BUTTON_DPAD_DOWN);
						P1.State[LEFT] = SDL_GameControllerGetButton(P1.GameController, SDL_CONTROLLER_BUTTON_DPAD_LEFT);
						P1.State[RIGHT] = SDL_GameControllerGetButton(P1.GameController, SDL_CONTROLLER_BUTTON_DPAD_RIGHT);
						P1.State[BUTTON_A] = SDL_GameControllerGetButton(P1.GameController, SDL_CONTROLLER_BUTTON_A);
						P1.State[BUTTON_B] = SDL_GameControllerGetButton(P1.GameController, SDL_CONTROLLER_BUTTON_B);
						P1.State[BUTTON_X] = SDL_GameControllerGetButton(P1.GameController, SDL_CONTROLLER_BUTTON_X);
						P1.State[BUTTON_Y] = SDL_GameControllerGetButton(P1.GameController, SDL_CONTROLLER_BUTTON_Y);
						P1.State[START] = SDL_GameControllerGetButton(P1.GameController, SDL_CONTROLLER_BUTTON_START);

						P1.LeftAxisX = SDL_JoystickGetAxis(P1.joy, SDL_CONTROLLER_AXIS_LEFTX);
						P1.LeftAxisY = SDL_JoystickGetAxis(P1.joy, SDL_CONTROLLER_AXIS_LEFTY);
						P1.Connected = true;

					}
					else
					{
						SDL_GameControllerClose(P1.GameController);
						P1.GameController = nullptr;
						P1.Connected = false;
					}
				}
			}
			else if (i < 1)
			{
				P2.Connected = false;
				SDL_GameControllerClose(P2.GameController);
				P2.GameController = nullptr;
			}
			else if (i == 1 || i == 0 && P1.Connected == false)
			{
				if (SDL_IsGameController(i))
				{
					P2.GameController = SDL_GameControllerOpen(1);
					P2.joy = SDL_JoystickOpen(1);
					if (SDL_GameControllerGetAttached(P2.GameController))
					{
						// CONTROLLER 2 ------------------------------------------------------------------------------------------

						P2.State[UP] = SDL_GameControllerGetButton(P2.GameController, SDL_CONTROLLER_BUTTON_DPAD_UP);
						P2.State[DOWN] = SDL_GameControllerGetButton(P2.GameController, SDL_CONTROLLER_BUTTON_DPAD_DOWN);
						P2.State[LEFT] = SDL_GameControllerGetButton(P2.GameController, SDL_CONTROLLER_BUTTON_DPAD_LEFT);
						P2.State[RIGHT] = SDL_GameControllerGetButton(P2.GameController, SDL_CONTROLLER_BUTTON_DPAD_RIGHT);
						P2.State[BUTTON_A] = SDL_GameControllerGetButton(P2.GameController, SDL_CONTROLLER_BUTTON_A);
						P2.State[BUTTON_B] = SDL_GameControllerGetButton(P2.GameController, SDL_CONTROLLER_BUTTON_B);
						P2.State[BUTTON_X] = SDL_GameControllerGetButton(P2.GameController, SDL_CONTROLLER_BUTTON_X);
						P2.State[BUTTON_Y] = SDL_GameControllerGetButton(P2.GameController, SDL_CONTROLLER_BUTTON_Y);
						P2.State[START] = SDL_GameControllerGetButton(P2.GameController, SDL_CONTROLLER_BUTTON_START);

						P2.LeftAxisX = SDL_JoystickGetAxis(P2.joy, SDL_CONTROLLER_AXIS_LEFTX);
						P2.LeftAxisY = SDL_JoystickGetAxis(P2.joy, SDL_CONTROLLER_AXIS_LEFTY);

						P2.Connected = true;
					}
					else
					{
						SDL_GameControllerClose(P2.GameController);
						P2.GameController = nullptr;
						P2.Connected = false;
					}
				}
			}
		}
	}

	for (int i = 0; i < MAX_BUTTONS; ++i)
	{
		if (P1.State[i] == 1) {
			if (P1.Controller[i] == KEY_IDLE)
				P1.Controller[i] = KEY_DOWN;
			else
				P1.Controller[i] = KEY_REPEAT;
		}
		else
		{
			if (P1.Controller[i] == KEY_REPEAT || P1.Controller[i] == KEY_DOWN)
				P1.Controller[i] = KEY_UP;
			else
				P1.Controller[i] = KEY_IDLE;
		}

		if (P2.State[i] == 1) {
			if (P2.Controller[i] == KEY_IDLE)
				P2.Controller[i] = KEY_DOWN;
			else
				P2.Controller[i] = KEY_REPEAT;
		}
		else
		{
			if (P2.Controller[i] == KEY_REPEAT || P2.Controller[i] == KEY_DOWN)
				P2.Controller[i] = KEY_UP;
			else
				P2.Controller[i] = KEY_IDLE;
		}

	}

	//CHECK Left Axis X & Y
	if (P1.LeftAxisX > DEATH_ZONE)
	{
		P1.Controller[JOY_RIGHT] = KEY_REPEAT;
	}
	else if (P1.LeftAxisX < -DEATH_ZONE)
	{
		P1.Controller[JOY_LEFT] = KEY_REPEAT;
	}

	if (P1.LeftAxisY < -DEATH_ZONE)
	{
		P1.Controller[JOY_UP] = KEY_REPEAT;
	}
	else if (P1.LeftAxisY > DEATH_ZONE)
	{
		P1.Controller[JOY_DOWN] = KEY_REPEAT;
	}

	//CHECK P2 Left Axis X & Y
	if (P2.LeftAxisX > DEATH_ZONE)
	{
		P2.Controller[JOY_RIGHT] = KEY_REPEAT;
	}
	else if (P2.LeftAxisX < -DEATH_ZONE)
	{
		P2.Controller[JOY_LEFT] = KEY_REPEAT;
	}

	if (P2.LeftAxisY < -DEATH_ZONE)
	{
		P2.Controller[JOY_UP] = KEY_REPEAT;
	}
	else if (P2.LeftAxisY > DEATH_ZONE)
	{
		P2.Controller[JOY_DOWN] = KEY_REPEAT;
	}


	while(SDL_PollEvent(&event) != 0)
	{
		switch(event.type)
		{
			case SDL_QUIT:
				windowEvents[WE_QUIT] = true;
			break;

			case SDL_WINDOWEVENT:
				switch(event.window.event)
				{
					//case SDL_WINDOWEVENT_LEAVE:
					case SDL_WINDOWEVENT_HIDDEN:
					case SDL_WINDOWEVENT_MINIMIZED:
					case SDL_WINDOWEVENT_FOCUS_LOST:
					windowEvents[WE_HIDE] = true;
					break;

					//case SDL_WINDOWEVENT_ENTER:
					case SDL_WINDOWEVENT_SHOWN:
					case SDL_WINDOWEVENT_FOCUS_GAINED:
					case SDL_WINDOWEVENT_MAXIMIZED:
					case SDL_WINDOWEVENT_RESTORED:
					windowEvents[WE_SHOW] = true;
					break;
				}
			break;

			case SDL_MOUSEBUTTONDOWN:
				mouse_buttons[event.button.button - 1] = KEY_DOWN;
				//LOG("Mouse button %d down", event.button.button-1);
			break;

			case SDL_MOUSEBUTTONUP:
				mouse_buttons[event.button.button - 1] = KEY_UP;
				//LOG("Mouse button %d up", event.button.button-1);
			break;

			case SDL_MOUSEMOTION:
				int scale = App->win->GetScale();
				mouse_motion_x = event.motion.xrel / scale;
				mouse_motion_y = event.motion.yrel / scale;
				mouse_x = event.motion.x / scale;
				mouse_y = event.motion.y / scale;
				//LOG("Mouse motion x %d y %d", mouse_motion_x, mouse_motion_y);
			break;
		}
	}

	return true;
}

// Called before quitting
bool Input::CleanUp()
{
	LOG("Quitting SDL event subsystem");
	SDL_QuitSubSystem(SDL_INIT_EVENTS);

	SDL_GameControllerClose(P1.GameController);
	P1.GameController = NULL;

	SDL_GameControllerClose(P2.GameController);
	P2.GameController = NULL;

	return true;
}

// ---------
bool Input::GetWindowEvent(j1EventWindow ev)
{
	return windowEvents[ev];
}

void Input::GetMousePosition(int& x, int& y)
{
	x = mouse_x;
	y = mouse_y;
}

void Input::GetMouseMotion(int& x, int& y)
{
	x = mouse_motion_x;
	y = mouse_motion_y;
}