#include "input.h"
#include "stages.h"

PauseStage::PauseStage(GameStage* _gamestage, float time) : gamestage(_gamestage), start_time(time)
{
	drawText(327, 500, "PAUSE", Vector3(1, 1, 1), 5);
}

void PauseStage::onGamepadButtonDown(SDL_JoyButtonEvent event)
{
	std::cout << "event.button: " << (int)event.button << std::endl;
	switch (event.button)
	{
	case START_BUTTON:
	{
		gamestage->pause_time += Game::instance->time - start_time;
		Game::instance->current_stage = gamestage;
	}
	}
}