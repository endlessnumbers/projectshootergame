#ifndef CUBE_H
#define CUBE_H
//GLM
#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <glm\gtc\type_ptr.hpp>
//GLEW
#define GLEW_STATIC
#include <glew\glew.h>
//SOIL
#include <SOIL.h>

class Cube
{
public:
	//functions
	Cube(glm::vec3 startingPos, glm::vec3 colour);
	//members
	void draw();
	int waypoint;
	glm::vec3 cubePos;
	glm::vec3 pickingColour;
	bool alive;
	double timeKilled;
	static GLfloat vertices[];
protected:
	GLuint VBO;
	GLuint containerVAO;
};

#endif