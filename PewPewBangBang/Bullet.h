#ifndef BULLET_H
#define BULLET_H
#define GLEW_STATIC
#include <glew\glew.h>
//GLM
#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <glm\gtc\type_ptr.hpp>
class Bullet
{
public:
	Bullet();
	Bullet(int bulletid, glm::vec3 cubePos);
	void calculateMovement();
	glm::vec3 bulletPos;
	bool alive;
private:
	int id;
};

#endif
