#include "animation.h"
#include "character.h"
#include "fbo.h"
#include "game.h"
#include "input.h"
#include "mesh.h"
#include "shader.h"
#include "stages.h"
#include "texture.h"
#include "utils.h"

#include <cmath>
#include "extra/bass.h"

Game* Game::instance = NULL;
int8_t Stage::nPlayers = 1;
Prop* selected = NULL;
bool seeGPUstats = false;

Game::Game(int window_width, int window_height, SDL_Window* window)
{
	this->window_width = window_width;
	this->window_height = window_height;
	this->window = window;
	instance = this;
	must_exit = false;

	fps = 0;
	frame = 0;
	time = 0.0f;
	elapsed_time = 0.0f;

	//OpenGL flags
	glEnable(GL_CULL_FACE); //render both sides of every triangle
	glEnable(GL_DEPTH_TEST); //check the occlusions using the Z buffer
	
	current_stage = new IntroStage();

	//Inicializamos BASS al arrancar el juego (id_del_device, muestras por segundo, ...)
	if (BASS_Init(-1, 44100, 0, 0, NULL) == false)
		std::cout << "Error initializing BASS" << std::endl;
}

//what to do when the image has to be draw
void Game::render(void)
{
	// Clear the window and the depth buffer
	glClear(GL_DEPTH_BUFFER_BIT);

	//FPS text stands for all window, independently of cameras
	glViewport(0, 0, window_width, window_height);

	//render the current stage
	current_stage->render();
	
	//render the FPS
	if(seeGPUstats)
		drawText(2, 2, getGPUStats(), Vector3(1, 1, 1), 2);

	//swap between front buffer and back buffer
	SDL_GL_SwapWindow(this->window);
}

void Game::update(double seconds_elapsed)
{
	current_stage->update(seconds_elapsed);
}

//Keyboard event handler (sync input)
void Game::onKeyDown( SDL_KeyboardEvent event )
{
	switch(event.keysym.sym)
	{
		case SDLK_g: seeGPUstats = !seeGPUstats; break;
		case SDLK_F1: Shader::ReloadAll(); break; 
	}
	current_stage->onKeyDown(event);
}

void Game::onKeyUp(SDL_KeyboardEvent event) {}

void Game::onGamepadButtonDown(SDL_JoyButtonEvent event)
{
	current_stage->onGamepadButtonDown(event);
}

void Game::onGamepadButtonUp(SDL_JoyButtonEvent event) {}

void Game::onMouseButtonDown( SDL_MouseButtonEvent event )
{
	current_stage->onMouseButtonDown(event);
}

void Game::onMouseButtonUp(SDL_MouseButtonEvent event) {}

void Game::onMouseWheel(SDL_MouseWheelEvent event) {}

void Game::onResize(int width, int height)
{
    std::cout << "window resized: " << width << "," << height << std::endl;
	glViewport( 0,0, width, height );
	window_width = width;
	window_height = height;
	current_stage->onResize(width,height);
}

