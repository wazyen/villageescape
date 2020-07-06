#include "input.h"
#include "stages.h"

enum Screens {COVER, STORY, CONTROLS};

void IntroStage::render()
{
	float hFactor = Game::instance->window_width / 800.0f,
		  vFactor = Game::instance->window_height / 600.0f;
	drawImage(screens[screenID]);
	switch (screenID)
	{
	case COVER:
	case STORY:
		drawText(Game::instance->window_width/2-210, Game::instance->window_height-50, "Press A to continue", Vector3(1, 1, 1), 4);
		break;
	case CONTROLS:
		drawText(Game::instance->window_width/2-210, Game::instance->window_height-50, "Press A to start", Vector3(1, 1, 1), 4);
		break;
	}
}

void IntroStage::update(float dt)
{
	for(int controller = 0; controller < 4; controller++)
		if (Input::wasButtonPressed(A_BUTTON, controller)) { screenID++; break; }
	if (!(screenID - OPENING_SCREENS)) Game::instance->current_stage = new LoadingStage();
}