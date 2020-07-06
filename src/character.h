#ifndef	CHARACTER_H
#define CHARACTER_H

#include "world.h"

class Prop
{
public:
	int type;
	Matrix44 model;
	Vector3 position;
	float yaw;
	Vector4 color;

	Prop()
	{
		type = 0;
		position = Vector3(0.0, 0.0, 0.0);
		yaw = 0;
		color.set(1, 1, 1, 1);
		model.setIdentity();
		model.translate(position.x, position.y, position.z);
		model.rotate(yaw*DEG2RAD, Vector3(0, 1, 0));
	}

	Prop(int type, Vector3 position, float yaw)
	{
		this->type = type;
		this->position = position;
		this->yaw = yaw;
		color.set(1,1,1,1);
		model.setIdentity();
		model.translate(position.x, position.y, position.z);
		model.rotate(yaw*DEG2RAD, Vector3(0, 1, 0));
	}
};

enum typeCharacter { AGENT = 1, PLAYER = 2 };
class Character : public Prop
{
public:
	Vector3 velocity;
	float pitch;
	float height;
	float idle_time;
	float run_time;
	float walk_time;
	int8_t controller;
	int8_t playerID;
	Camera* camera;

	std::vector<Matrix44> bone_matrices;

	Character(int type, Vector3 position, float yaw, int8_t playerID, int8_t controller);
	void render(Camera* camera, PropType pt);
	void update(float dt);
	void updateMatrix();
};

enum AgentState { IDLE, WALKINGA, WALKINGB, CAUGHT1, CAUGHT2};

class Agent : public Character
{
public:
	Vector3 spotA;
	Vector3 spotB;
	float idle_time = 0.0f;
	AgentState state = WALKINGB;

	Agent(float _yaw, Vector3 _spotA, Vector3 _spotB) :
		Character(AGENT, _spotA, _yaw, -1, -1), spotA(_spotA), spotB(_spotB) {};
};

#endif