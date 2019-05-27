#include "App.h"
#include "j1TransitionManager.h"
#include "j1Transitions.h"
#include "Render.h"
#include "Window.h"
#include "FadeToColor.h"
#include "Squares.h"
#include "Input.h"

//#include "MoveToMouse.h"
//#include "Zoom.h"
//#include "Wipe.h"
//#include "Lines.h"

j1TransitionManager::j1TransitionManager()
{}

j1TransitionManager::~j1TransitionManager()
{}

bool j1TransitionManager::PostUpdate() {

	// Calls PostUpdate (switch of states) of every transition in the list
	for (std::list<j1Transitions*>::iterator item = transitions_list.begin(); item != transitions_list.end(); ++item) {
		(*item)->PostUpdate();
	}
	return true;
}

bool j1TransitionManager::CleanUp() {

	// Clears the list
	transitions_list.clear();

	return true;
}

void j1TransitionManager::CleanTransitions(j1Transitions* transition) {

	// Removes the list and deletes the transition
	transitions_list.remove(transition);
	delete transition;
}

void j1TransitionManager::FadingToColor(j1Color color, float time) {
	transitions_list.push_back(new FadeToColor(color, time));
}

void j1TransitionManager::SquaresAppearing(int transition, j1Color color, float time) {
	transitions_list.push_back(new Squares(transition, color, time));
}