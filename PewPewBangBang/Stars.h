#ifndef STARS_H
#define STARS_H

#define GLEW_STATIC
#include <glew\glew.h>
//GLM
#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <glm\gtc\type_ptr.hpp>

class Stars
{
public:
	Stars();
	void Stars::draw();
	glm::vec3 starpos;
private:
	GLuint vbo;
	GLfloat vertices[];
};

#endif