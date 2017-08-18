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
	Cube(int ID, glm::vec3 startingPos, glm::vec3 colour);
	void draw();
	int cubeID;
	glm::vec3 cubePos;
	glm::vec3 pickingColour;
	bool alive;
	double timeKilled;
};

Cube::Cube(int ID, glm::vec3 startingPos, glm::vec3 colour)
{
	cubeID = ID;
	cubePos = startingPos;
	pickingColour = colour;
	alive = true;
	timeKilled = 0.0;
}
#endif