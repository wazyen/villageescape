#include "animation.h"
#include "character.h"
#include "stages.h"

#include "extra/bass.h"

void LoadingStage::render()
{
	Camera camera2D;
	camera2D.setOrthographic(0, Game::instance->window_width, 0, Game::instance->window_height, -1, 1);
	Mesh screen;
	screen.createQuad(Game::instance->window_width / 2, Game::instance->window_height / 2, Game::instance->window_width, Game::instance->window_height, false);
	Matrix44 model;
	Shader* shader = Shader::Get("data/shaders/basic.vs", "data/shaders/simpletexture.fs");
	shader->enable();
	shader->setUniform("u_model", model);
	shader->setUniform("u_viewprojection", camera2D.viewprojection_matrix);
	shader->setUniform("u_texture", Texture::Get("data/screens/loading.tga"));
	screen.render(GL_TRIANGLES);
	shader->disable();
}

void LoadingStage::update(float dt)
{
	// load meshes
	Mesh* mesh = Mesh::Get("data/meshes/house.obj");
	mesh = Mesh::Get("data/characters/male.mesh");
	mesh = Mesh::Get("data/meshes/wall.obj");
	mesh = Mesh::Get("data/meshes/sphere.obj");

	// load shaders
	Shader* shader = Shader::Get("data/shaders/basic.vs", "data/shaders/texture.fs");
	shader = Shader::Get("data/shaders/basic.vs", "data/shaders/flat.fs");
	shader = Shader::Get("data/shaders/basic.vs", "data/shaders/floor.fs");
	shader = Shader::Get("data/shaders/basic.vs", "data/shaders/sky.fs");
	shader = Shader::Get("data/shaders/skinning.vs", "data/shaders/phong.fs");
	shader = Shader::Get("data/shaders/basic.vs", "data/shaders/simpletexture.fs");

	// load textures
	Texture* texture = Texture::Get("data/characters/agent.tga");
	texture = Texture::Get("data/characters/prisoner.tga");
	texture = Texture::Get("data/textures/compass.tga");
	texture = Texture::Get("data/textures/grass.tga");
	texture = Texture::Get("data/textures/mask.tga");
	texture = Texture::Get("data/textures/materials.tga");
	texture = Texture::Get("data/textures/rocks.tga");
	texture = Texture::Get("data/textures/sky.tga");

	// load animations
	Animation* animation = Animation::Get("data/characters/idle.skanim");
	animation = Animation::Get("data/characters/running.skanim");
	animation = Animation::Get("data/characters/walking.skanim");

	Game::instance->current_stage = new GameStage();
}