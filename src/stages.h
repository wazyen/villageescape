#ifndef	STAGES_H
#define STAGES_H

#include "game.h"
#include "extra/bass.h"

#define OPENING_SCREENS 3

class Stage
{
public:
	static int8_t nPlayers;

	virtual void render() = 0;
	virtual void update(float dt) = 0;
	virtual void onMouseButtonDown(SDL_MouseButtonEvent event) {};
	virtual void onGamepadButtonDown(SDL_JoyButtonEvent event) {};
	virtual void onKeyDown(SDL_KeyboardEvent event) {};
	virtual void onResize(int width, int height) {};

	void drawImage(Texture* texture)
	{
		int width = Game::instance->window_width,
			height = Game::instance->window_height,
			imgWidth = width,
			imgHeight = height;
		if ((float)width / (float)height > 800.0f / 600.0f)
			imgWidth = (800.0f*height / 600.0f);
		else if ((float)width / (float)height < 800.0f / 600.0f)
			imgHeight = (600.0f*width / 800.0f);
		Camera camera2D;
		camera2D.setOrthographic(0, width, 0, height, -1, 1);
		Mesh screen;
		screen.createQuad(Game::instance->window_width / 2, Game::instance->window_height / 2, imgWidth, imgHeight, false);
		Matrix44 model;
		Shader* shader = Shader::Get("data/shaders/basic.vs", "data/shaders/simpletexture.fs");
		shader->enable();
		shader->setUniform("u_model", model);
		shader->setUniform("u_viewprojection", camera2D.viewprojection_matrix);
		shader->setUniform("u_texture", texture);
		screen.render(GL_TRIANGLES);
		shader->disable();
	};
};

class IntroStage : public Stage
{
	int8_t screenID = 0;
	Texture* screens[OPENING_SCREENS];
public:
	IntroStage()
	{
		for (int i = 0; i < OPENING_SCREENS; i++)
			screens[i] = Texture::Get(("data/screens/open" + std::to_string(i) + ".tga").c_str());
	};
	~IntroStage()
	{
		for (int i = 0; i < OPENING_SCREENS; i++)
			delete screens[i];
	};
	void render();
	void update(float dt);
};

class LoadingStage : public Stage
{
public:
	LoadingStage() {};
	~LoadingStage() {};
	void render();
	void update(float dt);
};

class GameStage : public Stage
{
	World* world;
	int window_width;
	int window_height;
	float start_time;
	bool skippedFrame = false;
public:
	HCHANNEL audio_grillos;
	HCHANNEL audio_tension;
	bool tension;
	float pause_time = 0.0f;
	bool gotCaught = false;
	Agent* agent = NULL;
	GameStage();
	~GameStage() { delete world; };
	void render();
	void update(float dt);
	void drawGUI();
	void onGamepadButtonDown(SDL_JoyButtonEvent event);
	void onKeyDown(SDL_KeyboardEvent event);
	void onResize(int width, int height);
};

class PauseStage : public Stage
{
	GameStage* gamestage;
	float start_time;
public:
	PauseStage(GameStage* _gamestage, float time);
	~PauseStage() {};
	void render() {};
	void update(float dt) {};
	void onGamepadButtonDown(SDL_JoyButtonEvent event);
};

class EndStage : public Stage
{
	bool isWin;
	float time;
	bool newHighscore = false;
public:
	EndStage(bool _isWin, float _time);
	~EndStage() {};
	void render();
	void update(float dt);
};

#endif