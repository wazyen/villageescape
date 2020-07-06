#ifndef WORLD_H
#define WORLD_H

#include <vector>

#include "framework.h"
#include "mesh.h"
#include "shader.h"
#include "texture.h"

class Agent;
class Camera;
class Character;
class Prop;

struct PropType
{
public:
	Mesh* mesh;
	Shader* shader;
	Texture* texture;

	void set(const char* mesh_name, const char* vertex_shader, const char* fragment_shader, const char* texture_name)
	{
		mesh = Mesh::Get(mesh_name);
		shader = Shader::Get(vertex_shader, fragment_shader);
		texture = Texture::Get(texture_name);
	};
};

enum { GRASS=0, ROCKS, MASK };

class World
{
private:
	World();

public:
	~World();

	static World* instance;
	static World* getInstance();

	Mesh* skysphere;
	Shader* skyshader;
	Texture* skytexture;
	Mesh* floormesh;
	Shader* floorshader;
	Texture* floortexture[3];

	Vector4 ambient_light;

	float yaw = 0.0;

	Character* player[2];
	PropType prop_types[16];
	std::vector<Prop*> props;
	std::vector<Character*> characters;
	std::vector<Agent*> agents;

	void generateWorld();
	void clearWorld();

	void render(Camera* camera);
	void update(float dt);

	Prop* spawnProp(Vector2 mouse, int type, Camera* camera);
	Character* spawnCharacter(Vector2 mouse, Camera* camera);
	void deleteProp(Vector2 mouse, Camera* camera);
	void save();
	void load();

	Prop* testRayProps(Vector3 origin, Vector3 dir);

	void switchController(int i);
};

#endif