#ifndef SHADER_H
#define SHADER_H

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <ctime>

#include <glew\glew.h>	//include glew to get openGL headers

class Shader
{
public:
	//program ID
	GLuint Program;
	//constructor reads and builds shader
	Shader(const GLchar* vertexPath, const GLchar* fragmentPath);
	//use the program
	void use();
};
#endif
