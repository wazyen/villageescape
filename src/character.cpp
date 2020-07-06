#include "character.h"

#include <algorithm>

#include "animation.h"
#include "game.h"
#include "includes.h"
#include "input.h"
#include "stages.h"

#include "extra/bass.h"

Character::Character(int type, Vector3 position, float yaw, int8_t playerID, int8_t controller) : Prop()
{
	pitch = 0.0;
	height = 10.0;
	idle_time = 0.0;
	walk_time = 0.0;
	run_time = 0.0;
	camera = new Camera();
	camera->lookAt(Vector3(0.f, 10.f, 50.f), Vector3(0.f, 0.f, 0.f), Vector3(0.f, 1.f, 0.f)); //position the camera and point to 0,0,0
	camera->setPerspective(70.f, Game::instance->window_width / (float)Game::instance->window_height, 0.1f, 10000.f); //set the projection, we want to be perspective
	camera->yaw = yaw;
	this->yaw = yaw;
	this->type = type;
	this->playerID = playerID;
	this->controller = controller;
	this->position = position;
	this->velocity = Vector3(0.0f,0.0f,0.0f);
}

void Character::updateMatrix()
{
	model.setIdentity();
	model.translate(position.x, position.y, position.z);
	model.rotate(yaw, Vector3(0, 1, 0));
}

void Character::render(Camera* camera, PropType pt)
{
	updateMatrix();
	Matrix44 m = model;
	float s = 0.5*height / 10.0;
	m.scale(s, s, s);

	Animation* walking = Animation::Get("data/characters/walking.skanim");
	walking->assignTime(Game::instance->time);
	float f = fmod(Game::instance->time, walking->duration) / walking->duration;
	Animation* running = Animation::Get("data/characters/running.skanim");
	running->assignTime(f*running->duration);
	Animation* idle = Animation::Get("data/characters/idle.skanim");
	idle->assignTime(Game::instance->time);

	Skeleton final_skeleton, cur_skeleton;
	float times[3] = { idle_time, walk_time, run_time };
	std::sort(times, times + 3, std::greater<float>());
	cur_skeleton = times[0] == idle_time ? idle->skeleton : (times[0] == walk_time ? walking->skeleton : running->skeleton);
	final_skeleton = times[1] == idle_time ? idle->skeleton : (times[1] == walk_time ? walking->skeleton : running->skeleton);

	blendSkeleton(&final_skeleton, &cur_skeleton, sin(clamp((4 * (Game::instance->time - times[1])), 0, 90 * DEG2RAD)), &final_skeleton);
	final_skeleton.updateGlobalMatrices();
	final_skeleton.computeFinalBoneMatrices(bone_matrices, pt.mesh);

	Shader* shader = pt.shader;
	shader->enable();
	shader->setUniform("u_color", World::instance->ambient_light);
	shader->setUniform("u_viewprojection", camera->viewprojection_matrix);
	shader->setUniform("u_model", m);
	shader->setUniform("u_texture", pt.texture);
	shader->setUniform("u_time", Game::instance->time);
	shader->setUniform("u_camera", camera->eye);
	shader->setMatrix44Array("u_bones", &bone_matrices[0], bone_matrices.size());

	pt.mesh->render(GL_TRIANGLES);
	shader->disable();
}

void Character::update(float dt)
{
	position.y = 0;
	Vector3 move = Vector3(0.0f, 0.0f, 0.0f);
	if (controller < 0) // Artificial Agents
	{
		Agent* agent = (Agent*)this;
		for (int i = 0; i < Stage::nPlayers; i++)
		{
			Vector3 playerPos = World::instance->player[i]->position;
			if ((playerPos - position).length() < 20) //BASS_ChannelPause(audio_tension);
				agent->state = (AgentState)(CAUGHT1 + i);
			if ((playerPos - position).length() < 50)
			{
				GameStage* gs = (GameStage*)Game::instance->current_stage;
				gs->tension = true;
			}
		}
		if (agent->state == IDLE)
		{
			if (agent->idle_time < Game::instance->time)
			{
				agent->yaw += 180.0f;
				agent->state = ((agent->spotA - agent->position).length() > (agent->spotB - agent->position).length() ? WALKINGA : WALKINGB);
			}
		}
		if (agent->state == WALKINGA)
		{
			if ((agent->spotA - agent->position).length() < 2)
			{
				agent->idle_time = Game::instance->time + 2.0f;
				agent->state = IDLE;
			}
			else
				move = (agent->spotA - agent->position).normalize()*0.5;
		}
		if (agent->state == WALKINGB)
		{
			if ((agent->spotB - agent->position).length() < 2)
			{
				agent->idle_time = Game::instance->time + 2.0f;
				agent->state = IDLE;
			}
			else
				move = (agent->spotB - agent->position).normalize()*0.5;
		}
		if (agent->state == CAUGHT1 || agent->state == CAUGHT2)
		{
			Vector3 playerPos = World::instance->player[agent->state - CAUGHT1]->position;
			Vector3 lookAt = (playerPos - position).normalize();
			float angle = acos(lookAt.z);
			yaw = angle + 2 * ceil(lookAt.x)*(PI - angle);
			float playerYaw = yaw + PI;
			World::instance->player[agent->state - CAUGHT1]->yaw = playerYaw;
			GameStage* gs = (GameStage*) Game::instance->current_stage;
			gs->gotCaught = true;
			gs->agent = agent;
		}
		if (move.x == 0 && move.z == 0) //Idle
			idle_time = Game::instance->time;
		else //Walking
			walk_time = Game::instance->time;
	}
	else // Gamepad-controlled player
	{
		float speed = 40.0f * (Input::gamepads[controller].axis[TRIGGER_RIGHT] + 2.0f);
		if (abs(Input::gamepads[controller].axis[LEFT_ANALOG_X]) > 0.25)
			move.x -= Input::gamepads[controller].axis[LEFT_ANALOG_X] * dt * speed;
		if (abs(Input::gamepads[controller].axis[LEFT_ANALOG_Y]) > 0.25)
			move.z -= Input::gamepads[controller].axis[LEFT_ANALOG_Y] * dt * speed;
		if (abs(Input::gamepads[controller].axis[RIGHT_ANALOG_X]) > 0.25)
			camera->yaw += Input::gamepads[controller].axis[RIGHT_ANALOG_X] * dt * 2.0f;
		if (abs(Input::gamepads[controller].axis[RIGHT_ANALOG_Y]) > 0.25)
			camera->pitch = clamp(camera->pitch - Input::gamepads[controller].axis[RIGHT_ANALOG_Y] * dt, -89*DEG2RAD, 89*DEG2RAD);

		if (move.x == 0 && move.z == 0) //Idle
			idle_time = Game::instance->time;
		else if (speed > 41.0f)
			run_time = Game::instance->time;
		else
			walk_time = Game::instance->time;
	}
	Matrix44 rot;
	if (type == PLAYER)
		rot.rotate(camera->yaw, Vector3(0, 1, 0));
	move = rot * move;
	velocity = velocity + move * 10;
	Vector3 old_position = position;
	position = position + velocity * dt;
	velocity *= 0.75f;
	if (velocity.length() > 0.1)
	{
		Vector3 v = velocity;
		v.normalize();
		float auxYaw = acos(-v.z);
		yaw = auxYaw * (2 * ceil(v.x*0.99) - 1) + PI;
	}

	//Collisions
	if (velocity.length() < 0.01 || type == 1)
		return;

	for (int i = 0; i < World::instance->characters.size(); i++)
	{
		Character* c = World::instance->characters[i];
		if (this == c)
			continue;
		Mesh* m = World::instance->prop_types[c->type].mesh;
		Vector3 character_center = position + Vector3(0, 4, 0);
		Vector3 coll; 	//temp var para guardar el mipunto de colision si lo hay
		Vector3 collnorm; 	//temp var para guardar la normal al punto de colision
		float max_ray_dist = 100;
		if (m->testSphereCollision(c->model, character_center, 3, coll, collnorm) == false)
			continue; //si no colisiona, pasamos al siguiente objeto
		Vector3 push_away = normalize(coll - character_center) * dt;
		position = old_position - push_away; //move to previous pos but a little bit further

		//cuidado con la Y, si nuestro juego es 2D la ponemos a 0
		position.y = 0;

		//reflejamos el vector velocidad para que de la sensacion de que rebota en la pared
		velocity = reflect(velocity, collnorm) * 0.95;
	}
	for (int i = 0; i < World::instance->props.size(); i++)
	{
		Prop* prop = World::instance->props[i];
		PropType pt = World::instance->prop_types[prop->type];
		Vector3 character_center = position + Vector3(0, 4, 0);
		Vector3 coll; 	//temp var para guardar el mipunto de colision si lo hay
		Vector3 collnorm; 	//temp var para guardar la normal al punto de colision
		float max_ray_dist = 100;
		if (pt.mesh->testSphereCollision(prop->model, character_center, 3, coll, collnorm) == false)
			continue; //si no colisiona, pasamos al siguiente objeto

		Vector3 push_away = normalize(coll - character_center) * dt;
		position = old_position - push_away; //move to previous pos but a little bit further

		//cuidado con la Y, si nuestro juego es 2D la ponemos a 0
		position.y = 0;

		//reflejamos el vector velocidad para que de la sensacion de que rebota en la pared
		velocity = reflect(velocity, collnorm) * 0.95;
	}
}