#include "animation.h"
#include "character.h"
#include "input.h"
#include "stages.h"

#include "extra/bass.h"

GameStage::GameStage()
{
	//El handler para un sample
	HSAMPLE hSample;

	//Cargamos un sample del disco duro (memoria, filename, offset, length, max, flags)
	//use BASS_SAMPLE_LOOP in the last param to have a looped sound
	hSample = BASS_SampleLoad(false, "data/audio/fondoGrillos.wav", 0, 0, 3, BASS_SAMPLE_LOOP);
	if (hSample == 0)
	{
		std::cout << "Error opening audio" << std::endl;
	}

	//Creamos un canal para el sample
	audio_grillos = BASS_SampleGetChannel(hSample, false);

	//Lanzamos un sample
	BASS_ChannelPlay(audio_grillos, true);

	hSample = BASS_SampleLoad(false, "data/audio/tension_v1.wav", 0, 0, 3, BASS_SAMPLE_LOOP);
	if (hSample == 0)
	{
		std::cout << "Error opening audio" << std::endl;
	}
	audio_tension = BASS_SampleGetChannel(hSample, false);
	BASS_ChannelPlay(audio_tension, true);
	BASS_ChannelPause(audio_tension);

	window_width = Game::instance->window_width;
	window_height = Game::instance->window_height;

	// generate world
	world = World::getInstance();
	world->generateWorld();

	start_time = Game::instance->time;
}

void GameStage::render()
{
	if (gotCaught)
	{
		agent->updateMatrix();
		Matrix44 model = agent->model;
		model.rotate(30*DEG2RAD, agent->camera->getLocalVector(Vector3(0.0f, 1.0f, 0.0f)));
		Vector3 eye = model * Vector3(0, 10, -20);
		Vector3 center = model * Vector3(0, 10, 0);
		agent->camera->lookAt(eye, center, Vector3(0, 1, 0));
		agent->camera->enable();

		glViewport(0, 0, window_width, window_height);
		agent->camera->aspect = window_width / (float)window_height;
		agent->camera->updateProjectionMatrix();
		world->render(agent->camera);
		return;
	}
	for (int i = 0; i < nPlayers; i++)
	{
		Character* player = world->player[i];
		player->updateMatrix();
		Matrix44 model;
		model.setIdentity();
		model.translate(player->position);
		model.rotate(player->camera->yaw, Vector3(0.0f, 1.0f, 0.0f));
		model.rotate(player->camera->pitch, /*camera->getLocalVector(*/Vector3(1.0f, 0.0f, 0.0f)/*)*/);
		Vector3 eye = model * Vector3(0.0f, 10.0f, -10.0f);
		Vector3 center = model * Vector3(0.0f, 10.0f, 20.0f);
		player->camera->lookAt(eye, center, Vector3(0, 1, 0));
		player->camera->enable();
		glViewport(i*window_width / Stage::nPlayers, 0, window_width / Stage::nPlayers, window_height);
		player->camera->aspect = window_width / ( (float)window_height * Stage::nPlayers );
		player->camera->updateProjectionMatrix();
		world->render(player->camera);
	}

	if (skippedFrame)
		return;

	//FPS text stands for all window, independently of cameras
	glViewport(0, 0, window_width, window_height);

	//draw time
	float time = Game::instance->time - start_time - pause_time;
	drawText(10, 40, std::to_string((int)(time)) + "." + std::to_string((int)((time)*10)%10), Vector3(1, 1, 1), 3);

	//draw GUI
	drawGUI();
}

void GameStage::drawGUI()
{
	Camera camera2D;
	camera2D.setOrthographic(0, window_width, 0, window_height, -1, 1);
	float hFactor = window_width / 800.0f,
		  vFactor = window_height / 600.0f;
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
	Mesh compass[2];
	for (int i = 0; i < nPlayers; i++)
	{
		float xPos = hFactor * (400.0f * ((i + 1.0f) + (2 - nPlayers)) - 55),
		      yPos = vFactor * (600.0f - 75);
		float width = min(hFactor * 100.0f, vFactor * 100.0f);
		compass[i].createQuad(xPos, yPos, width, width, false);
		Matrix44 model;
		Shader* shader = Shader::Get("data/shaders/basic.vs", "data/shaders/simpletexture.fs");
		shader->enable();
		shader->setUniform("u_model", model);
		shader->setUniform("u_viewprojection", camera2D.viewprojection_matrix);
		shader->setUniform("u_time", Game::instance->time);
		shader->setUniform("u_texture", Texture::Get("data/textures/compass.tga"));
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		compass[i].render(GL_TRIANGLES);
		shader->disable();
		glDisable(GL_BLEND);

		Character* player = world->player[i];
		Vector3 goal = Vector3(470.0f, 0.0f, 518.0f) - player->position;
		Mesh arrow;
		arrow.vertices.push_back(vFactor * Vector3(0, 40, 0));
		arrow.vertices.push_back(hFactor * Vector3(-4, 0, 0));
		arrow.vertices.push_back(hFactor * Vector3(4, 0, 0));
		arrow.vertices.push_back(hFactor * Vector3(4, 0, 0));
		arrow.vertices.push_back(hFactor * Vector3(-4, 0, 0));
		arrow.vertices.push_back(vFactor * Vector3(0, -4, 0));
		shader = Shader::Get("data/shaders/basic.vs", "data/shaders/flat.fs");
		shader->enable();
		model.setIdentity();
		model.translate(xPos,yPos, 0);
		model.rotate(atan(goal.z / goal.x), Vector3(0, 0, 1));
		if (goal.x < 0.0f)
			model.rotate(PI, Vector3(0, 0, 1));
		model.rotate(1.5*PI - player->camera->yaw, Vector3(0, 0, 1));
		shader->setUniform("u_model", model);
		shader->setUniform("u_viewprojection", camera2D.viewprojection_matrix);
		shader->setUniform("u_time", Game::instance->time);
		shader->setUniform("u_color", Vector4(1, 0, 0, 1));
		arrow.render(GL_TRIANGLES);
		shader->disable();
	}
	glEnable(GL_CULL_FACE);
}

void GameStage::update(float dt)
{
	world->update(dt);
	bool win = true;
	for (int i = 0; i < nPlayers; i++)
		if (world->player[i]->position.z < 500)
			win = false;
	if(win)
	{
		if (skippedFrame)
		{
			delete world;
			World::instance = NULL;
			Game::instance->current_stage = new EndStage(true, Game::instance->time - start_time);
		}
		else
			skippedFrame = true;
	}
	if (gotCaught)
	{
		if (skippedFrame)
		{
			delete world;
			World::instance = NULL;
			Game::instance->current_stage = new EndStage(false, Game::instance->time - start_time);
		}
		else
			skippedFrame = true;
	}
	if (tension)
	{
		GameStage* gs = (GameStage*)Game::instance->current_stage;
		BASS_ChannelPlay(gs->audio_tension, false);
		BASS_ChannelPause(gs->audio_grillos);
	}
	else
	{
		GameStage* gs = (GameStage*)Game::instance->current_stage;
		BASS_ChannelPlay(gs->audio_grillos, false);
		BASS_ChannelPause(gs->audio_tension);
	}
	tension = false;
}

void GameStage::onGamepadButtonDown(SDL_JoyButtonEvent event)
{
	std::cout << "event.button: " << (int)event.button << std::endl;
	switch (event.button)
	{
	case START_BUTTON: Game::instance->current_stage = new PauseStage(this, Game::instance->time); break;
	}
}

void GameStage::onKeyDown(SDL_KeyboardEvent event)
{
	switch (event.keysym.sym)
	{
	case SDLK_x:
	{
		world->switchController(0);
		break;
	}
	case SDLK_c:
	{
		world->switchController(1);
		break;
	}
	case SDLK_h: // HACK TO WIN FAST
	{
		if (Input::isKeyPressed(SDL_SCANCODE_LSHIFT))
		{
			for (int i = 0; i < nPlayers; i++)
			{
				world->player[i]->position = Vector3(475-i*10, 0, 480);
				world->player[i]->yaw = 0;
			}
		}
		break;
	}
	//case SDLK_p: Game::instance->current_stage = new PauseStage(this, Game::instance->time); break;
	}
}

void GameStage::onResize(int width, int height)
{
	window_width = width;
	window_height = height;
	for(int i = 0; i < nPlayers; i++)
		world->player[i]->camera->aspect = window_width / ((float)window_height * Stage::nPlayers);
};