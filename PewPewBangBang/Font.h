#ifndef FONT_H
#define FONT_H
//GLEW
#define GLEW_STATIC
#include <glew\glew.h>
//GLM
#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <glm\gtc\type_ptr.hpp>
//FREETYPE
#include <ft2build.h>
#include FT_FREETYPE_H
//standard includes
#include <iostream>
#include <map>
//local includes
#include "Shader.h"

class Font
{
public:
	Font();
	void renderText(Shader &s, std::string text, GLfloat x, GLfloat y, GLfloat scale, glm::vec3 colour);
	GLuint fontVAO;
	GLuint fontVBO;
private:
	//Struct to store generated data for each character loaded
	struct Character{
		GLuint textureID;
		glm::ivec2 size;	//size of glyph
		glm::ivec2 bearing;	//offset from baseline to top of glyph
		GLuint advance;		//offset to advance to next glyph
	};

	std::map<GLchar, Character> charMap;	//map stores key-value pairs; stores character data
};

#endif