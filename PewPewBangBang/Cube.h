#ifndef CUBE_H
#define CUBE_H
//GLM
#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <glm\gtc\type_ptr.hpp>
//GLEW
#define GLEW_STATIC
#include <glew\glew.h>
//GLFW
#include <GLFW\glfw3.h>
//SOIL
#include <SOIL.h>
//IRRKLANG
#include <irrklang\irrKlang.h>
#include <iostream>
//local includes
#include "Camera.h"
#include "Player.h"
#include "Bullet.h"

class Cube
{
public:
	//functions
	Cube();
	Cube(glm::vec3 startPos, glm::vec3 colour, int id);
	void calculateMovement(Player& player, Bullet bulletArray[]);
	//members
	int waypoint;
	glm::vec3 cubePos;
	glm::vec3 pickingColour;
	bool alive;
	double timeKilled;
	int id;
	irrklang::ISoundEngine *SoundEngine = irrklang::createIrrKlangDevice();
};

#endif