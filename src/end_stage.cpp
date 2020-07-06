#include "input.h"
#include "stages.h"

#include "extra/bass.h"

EndStage::EndStage(bool _isWin, float _time) : isWin(_isWin), time(_time)
{
	BASS_Stop();
	if ((Game::instance->highscore > _time || Game::instance->highscore==0) && _isWin)
	{
		Game::instance->highscore = _time;
		newHighscore = true;
	}
};

void EndStage::render()
{
	drawText(125, 80, "GAME OVER", Vector3(1, 1, 1), 10);
	if(newHighscore)
		drawText(200, 250, "NEW HIGHSCORE!", Vector3(1, 1, 1), 5);
	if(isWin)
		drawText(75, 350, "You completed the level in " + std::to_string((int)time) + " seconds", Vector3(1, 1, 1), 3);
	else
		drawText(75, 350, "Your time: " + std::to_string((int)time) + " seconds", Vector3(1, 1, 1), 3);
	if(Game::instance->highscore > 0.0f)
		drawText(75, 400, "Highscore: " + std::to_string((int)Game::instance->highscore) + " seconds", Vector3(1, 1, 1), 3);
	drawText(190, 500, "Press Z to start a new game", Vector3(1, 1, 1), 3);
	drawText(285, 550, "Press X to quit", Vector3(1, 1, 1), 3);
}

void EndStage::update(float dt)
{
	if (Input::wasKeyPressed(SDL_SCANCODE_Z)) //if key Z was pressed
	{
		BASS_Start();
		Game::instance->current_stage = new GameStage(); //start new game
	}
	if (Input::wasKeyPressed(SDL_SCANCODE_X)) //if key X was pressed
		Game::instance->must_exit = true; //exit game
}