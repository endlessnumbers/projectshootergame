#include "Crosshair.h"

//left
GLfloat crosshair1[] = {
	// positions         // colors
	0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,   // bottom right
	-0.02f, 0.01f, 0.0f, 1.0f, 0.0f, 0.0f,   // bottom left
	-0.01f, 0.03f, 0.0f, 1.0f, 0.0f, 0.0f    // top 
};
//bottom
GLfloat crosshair2[] = {
	// positions         // colors
	0.01f, -0.02f, 0.0f, 1.0f, 0.0f, 0.0f,   // bottom right
	-0.01f, -0.02f, 0.0f, 1.0f, 0.0f, 0.0f,   // bottom left
	0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f    // top 
};
//right
GLfloat crosshair3[] = {
	// positions         // colors
	0.02f, 0.01f, 0.0f, 1.0f, 0.0f, 0.0f,   // bottom right
	0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,   // bottom left
	0.01f, 0.03f, 0.0f, 1.0f, 0.0f, 0.0f    // top 
};

Crosshair::Crosshair()
{
	//crosshair
	glGenVertexArrays(1, &crosshair1VAO);
	glGenBuffers(1, &crosshair1VBO);
	glBindVertexArray(crosshair1VAO);
	glBindBuffer(GL_ARRAY_BUFFER, crosshair1VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(crosshair1), crosshair1, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);
	//colour
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	//crosshair
	glGenVertexArrays(1, &crosshair2VAO);
	glGenBuffers(1, &crosshair2VBO);
	glBindVertexArray(crosshair2VAO);
	glBindBuffer(GL_ARRAY_BUFFER, crosshair2VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(crosshair2), crosshair2, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);
	//colour
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	//crosshair
	glGenVertexArrays(1, &crosshair3VAO);
	glGenBuffers(1, &crosshair3VBO);
	glBindVertexArray(crosshair3VAO);
	glBindBuffer(GL_ARRAY_BUFFER, crosshair3VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(crosshair3), crosshair3, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);
	//colour
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void Crosshair::draw(Shader& triShader)
{
	//draw crosshair
	triShader.use();
	glBindVertexArray(crosshair1VAO);
	glDrawArrays(GL_TRIANGLES, 0, 3);

	triShader.use();
	glBindVertexArray(crosshair2VAO);
	glDrawArrays(GL_TRIANGLES, 0, 3);

	triShader.use();
	glBindVertexArray(crosshair3VAO);
	glDrawArrays(GL_TRIANGLES, 0, 3);
}
