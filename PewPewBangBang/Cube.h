#ifndef CUBE_H
#define CUBE_H
#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <glm\gtc\type_ptr.hpp>
#define GLEW_STATIC
#include <glew\glew.h>

class Cube
{
public:
	Cube(glm::vec3 startingPos, glm::vec3 colour);
	void draw();
	int waypoint;
	glm::vec3 cubePos;
	glm::vec3 pickingColour;
	bool alive;
	double timeKilled;
};

Cube::Cube(glm::vec3 startingPos, glm::vec3 colour)
{
	cubePos = startingPos;
	pickingColour = colour;
	alive = true;
	timeKilled = 0.0;
	waypoint = 1;
}
#endif