#ifndef LASER_H
#define LASER_H
#include "Shader.h"

class Laser
{
public:
	Laser();
	void draw(Shader& triangleShader);
private:
	GLuint laserVAO, laserVBO;
};

#endif