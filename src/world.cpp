#include "world.h"

#include <fstream>
#include <algorithm>
#include <cstring>

#include "animation.h"
#include "camera.h"
#include "character.h"
#include "game.h"
#include "includes.h"
#include "input.h"
#include "stages.h"
#include "utils.h"

#include <cassert>

#define MAX_VISIBILITY_DISTANCE 300
#define MAX_CHARACTERS 20

World* World::instance = NULL;

World* World::getInstance()
{
	if (instance == NULL)
		instance = new World();
	return instance;
}

World::World()
{
	instance = this;
	ambient_light = Vector4(0.2, 0.3, 0.7, 1.0);
}

World::~World()
{
	clearWorld();
}

void World::generateWorld()
{
	prop_types[0].set("data/meshes/house.obj", "data/shaders/basic.vs", "data/shaders/texture.fs", "data/textures/materials.tga");
	prop_types[1].set("data/characters/male.mesh", "data/shaders/skinning.vs", "data/shaders/phong.fs", "data/characters/agent.tga");
	prop_types[2].set("data/characters/male.mesh", "data/shaders/skinning.vs", "data/shaders/phong.fs", "data/characters/prisoner.tga");
	prop_types[3].set("data/meshes/wall.obj", "data/shaders/basic.vs", "data/shaders/texture.fs", "data/textures/materials.tga");
	prop_types[4].set("data/meshes/wall.obj", "data/shaders/basic.vs", "data/shaders/texture.fs", "data/textures/materials.tga");

	skysphere = Mesh::Get("data/meshes/sphere.obj");
	skyshader = Shader::Get("data/shaders/basic.vs", "data/shaders/sky.fs");
	skytexture = Texture::Get("data/textures/sky.tga");

	floormesh = new Mesh();
	floormesh->createPlane(500);
	floorshader = Shader::Get("data/shaders/basic.vs", "data/shaders/floor.fs");
	floortexture[GRASS] = Texture::Get("data/textures/grass.tga");
	floortexture[ROCKS] = Texture::Get("data/textures/rocks.tga");
	floortexture[MASK] = Texture::Get("data/textures/mask.tga");

	load();
}

void World::clearWorld()
{
	for (int i = 0; i < props.size(); i++)
		delete props[i];
	props.resize(0);
	for (int i = 0; i < characters.size(); i++)
		delete characters[i];
	characters.resize(0);
}

void World::render(Camera* camera)
{
	glDisable(GL_BLEND);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);

	// draw sky sphere
	Matrix44 m;
	m.translate(camera->eye.x, camera->eye.y, camera->eye.z);
	skyshader->enable();
	skyshader->setUniform("u_color", ambient_light);
	skyshader->setUniform("u_viewprojection", camera->viewprojection_matrix);
	skyshader->setUniform("u_texture", skytexture);
	skyshader->setUniform("u_time", Game::instance->time);
	skyshader->setUniform("u_camera", camera->eye);
	skyshader->setUniform("u_model", m);
	skysphere->render(GL_TRIANGLES);
	skyshader->disable();

	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);

	Shader* shader;
	PropType pt;
	for (int i = 0; i < props.size(); i++)
	{
		if(props[i]->type == 4)
			continue;
		Vector3 meshpos = props[i]->position;
		Vector3 eyepos = camera->eye;
		if ((meshpos-eyepos).length() > MAX_VISIBILITY_DISTANCE)
			continue;
		pt = prop_types[props[i]->type];
		BoundingBox box = transformBoundingBox(props[i]->model, pt.mesh->box);
		if ((uint8_t)camera->testBoxInFrustum(box.center, box.halfsize) == CLIP_OUTSIDE)
			continue;
		shader = pt.shader;
		shader->enable();
		Vector4 u_color = Vector4(props[i]->color.x*ambient_light.x, props[i]->color.y*ambient_light.y, props[i]->color.z*ambient_light.z, props[i]->color.w*ambient_light.w);
		shader->setUniform("u_color", u_color);
		shader->setUniform("u_viewprojection", camera->viewprojection_matrix);
		shader->setUniform("u_texture", pt.texture);
		shader->setUniform("u_time", Game::instance->time);
		shader->setUniform("u_camera", camera->eye);
		shader->setUniform("u_model", props[i]->model);
		pt.mesh->render(GL_TRIANGLES);
		shader->disable();
	}
	m.setIdentity();
	floorshader->enable();
	floorshader->setUniform("u_color", ambient_light);
	floorshader->setUniform("u_viewprojection", camera->viewprojection_matrix);
	floorshader->setUniform("u_texture_grass", floortexture[GRASS]);
	floorshader->setUniform("u_texture_rocks", floortexture[ROCKS]);
	floorshader->setUniform("u_texture_mask", floortexture[MASK]);
	floorshader->setUniform("u_time", Game::instance->time);
	floorshader->setUniform("u_camera", camera->eye);
	floorshader->setUniform("u_model", m);

	floormesh->render(GL_TRIANGLES);
	
	//disable shader
	floorshader->disable();
	for (int i = 0; i < characters.size(); i++)
	{
		if ((characters[i]->position - camera->eye).length() < MAX_VISIBILITY_DISTANCE)
			characters[i]->render(camera, prop_types[characters[i]->type]);
	}
}

void World::update(float dt)
{
	for (int i = 0; i < characters.size(); i++)
		characters[i]->update(dt);
}

Prop* World::spawnProp(Vector2 mouse, int type, Camera* camera)
{
	Vector3 dir = camera->getRayDirection(mouse.x, mouse.y, Game::instance->window_width/2, Game::instance->window_height);
	Vector3 pos = RayPlaneCollision(Vector3(), Vector3(0, 1, 0), camera->eye, dir);
	Prop* prop = new Prop(type, Vector3(pos.x, 0, pos.z), round(player[0]->yaw/90)*90);
	props.push_back(prop);
	return prop;
}

Character* World::spawnCharacter(Vector2 mouse, Camera* camera)
{
	Vector3 dir = camera->getRayDirection(mouse.x, mouse.y, Game::instance->window_width / 2, Game::instance->window_height);
	Vector3 origin = camera->eye;
	Vector3 pos = RayPlaneCollision(Vector3(), Vector3(0, 1, 0), camera->eye, dir);
	Character* c = new Character((rand() % 2)+1, pos, rand() % 360, -1, -1);
	characters.push_back(c);
	return c;
}

void World::deleteProp(Vector2 mouse, Camera* camera)
{
	Vector3 dir = camera->getRayDirection(mouse.x, mouse.y, Game::instance->window_width/2, Game::instance->window_height);
	Vector3 origin = camera->eye;
	Prop* selected = testRayProps(origin, dir);
	std::vector<Prop*>::iterator it = std::find(props.begin(), props.end(), selected);
	if (it != props.end())
	{
		std::cout << "Element Found" << std::endl;
		props.erase(it);
	}
	else
		std::cout << "Element Not Found" << std::endl;
}

void World::save()
{
	FILE* file = fopen("data/world/props.txt", "w");
	for (int i = 0; i < props.size(); i++)
		fprintf(file, "%d %f %f %f\n", props[i]->type, props[i]->position.x, props[i]->position.z, props[i]->yaw);
	fclose(file);
	file = fopen("data/world/players.txt", "w");
	for (int i = 0; i < 2; i++)
		fprintf(file, "%f %f %f\n", characters[i]->position.x, characters[i]->position.z, characters[i]->yaw);
	fclose(file);
	file = fopen("data/world/agents.txt", "w");
	for (int i = 2; i < characters.size(); i++)
	{
		Agent* a = (Agent*)characters[i];
		fprintf(file, "%f %f %f %f %f\n", a->yaw, a->spotA.x, a->spotA.z, a->spotB.x, a->spotB.z);
	}
	fclose(file);
}

void World::load()
{
	clearWorld();
	std::ifstream props_file("data/world/props.txt");
	int type;
	float posx, posz, yaw;
	while (props_file >> type >> posx >> posz >> yaw)
		props.push_back(new Prop(type, Vector3(posx, 0, posz), yaw));
	std::ifstream players_file("data/world/players.txt");
	int nPlayers;
	while (players_file >> nPlayers >> posx >> posz >> yaw)
		if(nPlayers==Stage::nPlayers)
			characters.push_back(new Character(PLAYER, Vector3(posx, 0, posz), yaw, -1, -1));
	for (int i = 0; i < Stage::nPlayers; i++)
	{
		player[i]=characters[i];
		player[i]->playerID = i;
		player[i]->controller = i;
	}
	std::ifstream agents_file("data/world/agents.txt");
	float spotAx, spotAz, spotBx, spotBz;
	while (agents_file >> yaw >> spotAx >> spotAz >> spotBx >> spotBz)
		agents.push_back(new Agent(yaw, Vector3(spotAx, 0.0f, spotAz), Vector3(spotBx, 0.0f, spotBz)));
	for (int i = 0; i < agents.size(); i++)
		characters.push_back((Character*)agents[i]);
}

Prop* World::testRayProps(Vector3 origin, Vector3 dir)
{
	Vector3 coll, collnormal;
	Prop* selected = NULL;
	float dist = MAX_VISIBILITY_DISTANCE;
	for (int i = 0; i < props.size(); i++)
	{
		Prop* p = props[i];
		assert(p->type != -1);
		if (p->type == -1)
			continue;
		Mesh* mesh = prop_types[p->type].mesh;

		if (mesh->testRayCollision(p->model, origin, dir, coll, collnormal) == false)
		{
			continue;
		}
		if(coll.distance(origin) < dist)
		{
			selected = p;
			dist = coll.distance(origin);
		}
	}
	return selected;
}

void World::switchController(int i)
{
		player[i]->controller = (player[i]->controller+1)%4;
}
