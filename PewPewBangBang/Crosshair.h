#define GLEW_STATIC
#ifndef CROSSHAIR_H
#define CROSSHAIR_H
#include <glew\glew.h>
#include "Shader.h"

class Crosshair
{
public:
	Crosshair();
	void draw(Shader& triShader);
private:
	GLuint crosshair1VAO, crosshair1VBO;
	GLuint crosshair2VAO, crosshair2VBO;
	GLuint crosshair3VAO, crosshair3VBO;
};

#endif