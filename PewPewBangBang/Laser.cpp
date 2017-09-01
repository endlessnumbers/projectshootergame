#include "Laser.h"

GLfloat laserVerts[] = {
	// positions         // colors
	0.03f, -1.0f, 0.0f, 1.0f, 0.0f, 0.0f,   // bottom right
	-0.03f, -1.0f, 0.0f, 1.0f, 0.0f, 0.0f,   // bottom left
	0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f    // top 
};

Laser::Laser()
{
	//laser
	glGenVertexArrays(1, &laserVAO);
	glGenBuffers(1, &laserVBO);
	glBindVertexArray(laserVAO);
	glBindBuffer(GL_ARRAY_BUFFER, laserVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(laserVerts), laserVerts, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);
	//colour
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}


void Laser::draw(Shader& triangleShader)
{
	triangleShader.use();
	glBindVertexArray(laserVAO);
	glDrawArrays(GL_TRIANGLES, 0, 3);
}
