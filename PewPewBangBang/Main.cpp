#define GLEW_STATIC
#include <glew\glew.h>
#include <GLFW\glfw3.h>
//GLM
#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <glm\gtc\type_ptr.hpp>
//FREETYPE
#include <ft2build.h>
#include FT_FREETYPE_H
//IRRKLANG
#include <irrklang\irrKlang.h>
//LOCAL
#include "Camera.h"
#include "Shader.h"
#include "Cube.h"
//C++ STANDARD
#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <map>
#include <random>
using namespace irrklang;

// Function prototypes
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void click_callback(GLFWwindow* window, int button, int action, int mods);
void do_movement();
void drawHUD();
void loadFont();
void renderText(Shader &s, std::string text, GLfloat x, GLfloat y, GLfloat scale, glm::vec3 colour);
void gameOver();
void calculateMovement(Cube &current, int i);
void drawLaser();

// Window dimensions
const GLuint WIDTH = 800, HEIGHT = 600;

//Struct to store generated data for each character loaded
struct Character{
	GLuint textureID;
	glm::ivec2 size;	//size of glyph
	glm::ivec2 bearing;	//offset from baseline to top of glyph
	GLuint advance;		//offset to advance to next glyph
};


std::map<GLchar, Character> charMap;	//map stores key-value pairs; stores character data
//these should be moved later!
GLuint fontVAO;
GLuint fontVBO;

bool keys[1024];
GLfloat deltaTime = 0.0f;	//time between current frame and last frame
GLfloat lastFrame = 0.0f;	//time of last frame

Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
GLfloat lastX = WIDTH / 2.0;
GLfloat lastY = HEIGHT / 2.0;

//light vector
glm::vec3 lightPos(1.2f, 1.0f, 2.0f);
//cube vector
//glm::vec3 cubePos(0.0f, 0.0f, -7.2f);

//CREATE CUBES
glm::vec3 cubeStartPos[] = {
	glm::vec3(0.0f, 0.0f, -7.2f),
	glm::vec3(-2.1f, 1.4f, -8.0f),
	glm::vec3(3.4f, -0.8f, -6.4f)
};

Cube cubeArray[] = {
	Cube(0, glm::vec3(0.0f, 0.0f, -7.2f), glm::vec3(0.0f, 1.0f, 0.0f)),	//green
	Cube(1, cubeStartPos[1], glm::vec3(0.0f, 0.0f, 1.0f)),	//blue
	Cube(2, cubeStartPos[2], glm::vec3(1.0f, 0.0f, 0.0f))	//red
};

//open log file for debugging output
std::ofstream logFile;

//movement
int waypoint = 1;
int score = 0;

int playerHealth = 10;

ISoundEngine *SoundEngine = createIrrKlangDevice();

int main()
{
	// Init GLFW
	glfwInit();
	// Set all the required options for GLFW
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

	// Create a GLFWwindow object that we can use for GLFW's functions
	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "PewPewBangBang", nullptr, nullptr);
	glfwMakeContextCurrent(window);

	// Set the required callback functions
	glfwSetKeyCallback(window, key_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);

	//activate mouse input; glfw will capture the cursor
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// Set this to true so GLEW knows to use a modern approach to retrieving function pointers and extensions
	glewExperimental = GL_TRUE;
	// Initialize GLEW to setup the OpenGL Function pointers
	glewInit();

	// Define the viewport dimensions
	glViewport(0, 0, WIDTH, HEIGHT);

	//enable depth testing
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	//enable blending
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	//setup shaders
	Shader lightShader("light.vs", "light.frag");
	Shader lampShader("lamp.vs", "lamp.frag");
	Shader fontShader("font.vs", "font.frag");
	Shader pickingShader("picking.vs", "picking.frag");
	Shader particleShader("particle.vs", "particle.frag");
	Shader laserShader("laser.vs", "laser.frag");

	camera.MouseSensitivity = 0.05f;

	//prepare font shaders
	glm::mat4 fontProjection = glm::ortho(0.0f, static_cast<GLfloat>(WIDTH), 0.0f,
		static_cast<GLfloat>(HEIGHT));
	fontShader.use();
	glUniformMatrix4fv(glGetUniformLocation(fontShader.Program, "projection"), 1, GL_FALSE,
		glm::value_ptr(fontProjection));

	//load the font
	loadFont();

	GLfloat vertices[] = {
		-0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f,
		0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f,
		0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f,
		0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f,
		-0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f,
		-0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f,

		-0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f,
		0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f,
		0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f,
		0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f,
		-0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f,
		-0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f,

		-0.5f, 0.5f, 0.5f, -1.0f, 0.0f, 0.0f,
		-0.5f, 0.5f, -0.5f, -1.0f, 0.0f, 0.0f,
		-0.5f, -0.5f, -0.5f, -1.0f, 0.0f, 0.0f,
		-0.5f, -0.5f, -0.5f, -1.0f, 0.0f, 0.0f,
		-0.5f, -0.5f, 0.5f, -1.0f, 0.0f, 0.0f,
		-0.5f, 0.5f, 0.5f, -1.0f, 0.0f, 0.0f,

		0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f,
		0.5f, 0.5f, -0.5f, 1.0f, 0.0f, 0.0f,
		0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f,
		0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f,
		0.5f, -0.5f, 0.5f, 1.0f, 0.0f, 0.0f,
		0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f,

		-0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f,
		0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f,
		0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f,
		0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f,
		-0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f,
		-0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f,

		-0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
		0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
		0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f,
		0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f,
		-0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f,
		-0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f
	};


	GLfloat laserVerts[] = {
		// positions         // colors
		0.03f, -1.0f, 0.0f, 1.0f, 0.0f, 0.0f,   // bottom right
		-0.03f, -1.0f, 0.0f, 1.0f, 0.0f, 0.0f,   // bottom left
		0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f    // top 
	};

	//open logfile
	logFile.open("log.txt", std::ofstream::app);

	GLuint VBO, containerVAO;
	glGenVertexArrays(1, &containerVAO);
	glBindVertexArray(containerVAO);
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	//position attributes of cube
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GL_FLOAT), (GLvoid*)0);
	glEnableVertexAttribArray(0);
	//normal attributes
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);
	glBindVertexArray(0);

	GLuint lightVAO;

	glGenVertexArrays(1, &lightVAO);
	glBindVertexArray(lightVAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	//lamp - same shape as the other cube!
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GL_FLOAT), (GLvoid*)0);
	glEnableVertexAttribArray(0);
	glBindVertexArray(0);

	//VAO/VBOs for texture quads
	glGenVertexArrays(1, &fontVAO);
	glGenBuffers(1, &fontVBO);
	glBindVertexArray(fontVAO);
	glBindBuffer(GL_ARRAY_BUFFER, fontVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	//laser
	GLuint laserVAO, laserVBO;
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

	//game loop
	while (!glfwWindowShouldClose(window))
	{
		glfwPollEvents();

		//PICKING
		glfwSetMouseButtonCallback(window, GLFW_MOUSE_BUTTON_1);
			
		//clear screen in white, for picking
		glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		pickingShader.use();

		for (int i = 0; i < 3; i++)
		{
			//respawn cubes
			if (!cubeArray[i].alive && (glfwGetTime() - cubeArray[i].timeKilled) >= 10.0)
			{
				cubeArray[i].alive = true;
			}

			if (cubeArray[i].alive)	//only do them if they're alive or respawn them
			{
				//calculate movement - they should only move and shoot if they're alive!
				calculateMovement(cubeArray[i], cubeArray[i].cubeID);

				GLint colourLoc = glGetUniformLocation(pickingShader.Program, "objectColor");
				glUniform3f(colourLoc, cubeArray[i].pickingColour.x, cubeArray[i].pickingColour.y, cubeArray[i].pickingColour.z);

				//camera transformations
				glm::mat4 pickView;
				pickView = camera.GetViewMatrix();
				glm::mat4 pickProjection = glm::perspective(camera.Zoom, (GLfloat)WIDTH / (GLfloat)HEIGHT, 0.1f, 100.0f);
				//get the uniform locations
				GLint modelLoc = glGetUniformLocation(pickingShader.Program, "model");
				GLint viewLoc = glGetUniformLocation(pickingShader.Program, "view");
				GLint projLoc = glGetUniformLocation(pickingShader.Program, "projection");
				//pass matrices to shader
				glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(pickView));
				glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(pickProjection));

				//draw container
				glBindVertexArray(containerVAO);
				glm::mat4 pickCubeModel;
				pickCubeModel = glm::mat4();
				pickCubeModel = glm::translate(pickCubeModel, cubeArray[i].cubePos);
				glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(pickCubeModel));
				glDrawArrays(GL_TRIANGLES, 0, 36);
				glBindVertexArray(0);
			}
		}

		//picking the colour
		glFlush();
		glFinish();
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

		int mouseButton = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT);
		if (mouseButton)
		{
			//laser sound
			SoundEngine->play2D("audio/bell.wav", GL_FALSE);

			//read the pixel at the centre of the screen
			unsigned char data[4];
			glReadPixels(WIDTH / 2, HEIGHT / 2, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, data);

			int pickedID =
				data[0] +
				data[1] * 256 +
				data[2] * 256 * 256;

			std::cout << pickedID << std::endl;

			if (pickedID == 0x00ffffff)
			{
				renderText(fontShader, "BACKGROUND", 540.0f, 570.0f, 0.5f, glm::vec3(0.3f, 0.7f, 0.9f));
			}
			else if (pickedID == 0x0000ff00)
			{
				renderText(fontShader, "CUBE GREEN", 540.0f, 570.0f, 0.5f, glm::vec3(0.3f, 0.7f, 0.9f));
				cubeArray[0].alive = false;
				cubeArray[0].timeKilled = glfwGetTime();
				score += 100;
			}
			else if (pickedID == 0x00ff0000)
			{
				renderText(fontShader, "CUBE BLUE", 540.0f, 570.0f, 0.5f, glm::vec3(0.3f, 0.7f, 0.9f));
				cubeArray[1].alive = false;
				cubeArray[1].timeKilled = glfwGetTime();
				score += 100;
			}
			else if (pickedID == 0x000000ff)
			{
				renderText(fontShader, "CUBE RED", 540.0f, 570.0f, 0.5f, glm::vec3(0.3f, 0.7f, 0.9f));
				cubeArray[2].alive = false;
				cubeArray[2].timeKilled = glfwGetTime();
				score += 100;
			}
		}
		
		//END PICKING
		
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		std::ostringstream scoreText;
		scoreText << "Score: " << score;

		//check if player is dead
		if (playerHealth <= 0)
		{
			glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
			renderText(fontShader, "Game Over!", 30.0f, 300.0f, 2.0f, glm::vec3(1.0f, 0.0f, 0.0f));
			renderText(fontShader, "Press Esc to Exit", 30.0f, 100.0f, 1.0f, glm::vec3(1.0f, 0.0f, 0.0f));
			renderText(fontShader, scoreText.str(), 30.0f, 20.0f, 0.7f, glm::vec3(1.0f, 0.0f, 0.0f));
			glfwSwapBuffers(window);
		}
		else
		{
			do_movement();

			GLfloat currentFrame = glfwGetTime();
			deltaTime = currentFrame - lastFrame;
			lastFrame = currentFrame;

			std::ostringstream healthText;
			healthText << "Health: " << playerHealth;

			renderText(fontShader, healthText.str(), 25.0f, 25.0f, 1.0f, glm::vec3(0.5f, 0.8f, 0.2f));
			renderText(fontShader, scoreText.str(), 540.0f, 570.0f, 0.5f, glm::vec3(0.3f, 0.7f, 0.9f));
		
			//set uniforms and draw objects
			lightShader.use();
			GLint objectColorLoc = glGetUniformLocation(lightShader.Program, "objectColor");
			GLint lightColorLoc = glGetUniformLocation(lightShader.Program, "lightColor");
			GLint lightPosLoc = glGetUniformLocation(lightShader.Program, "lightPos");
			GLint viewPosLoc = glGetUniformLocation(lightShader.Program, "viewPos");
			glUniform3f(viewPosLoc, camera.Position.x, camera.Position.y, camera.Position.z);
			glUniform3f(lightPosLoc, lightPos.x, lightPos.y, lightPos.z);
			glUniform3f(objectColorLoc, 1.0f, 0.5f, 0.31f);
			glUniform3f(lightColorLoc, 1.0f, 1.0f, 1.0f);

			//camera transformations
			glm::mat4 view;
			view = camera.GetViewMatrix();
			glm::mat4 projection = glm::perspective(camera.Zoom, (GLfloat)WIDTH / (GLfloat)HEIGHT, 0.1f, 100.0f);
			//get the uniform locations
			GLint modelLoc = glGetUniformLocation(lightShader.Program, "model");
			GLint viewLoc = glGetUniformLocation(lightShader.Program, "view");
			GLint projLoc = glGetUniformLocation(lightShader.Program, "projection");
			//pass matrices to shader
			glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
			glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

			for (int i = 0; i < 3; i++)
			{
				if (cubeArray[i].alive)
				{
					//draw container
					glBindVertexArray(containerVAO);
					glm::mat4 cubemodel;
					cubemodel = glm::mat4();
					cubemodel = glm::translate(cubemodel, cubeArray[i].cubePos);
					glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(cubemodel));
					glDrawArrays(GL_TRIANGLES, 0, 36);
					glBindVertexArray(0);
				}
			}

			//draw lamp
			lampShader.use();
			//get location objects for matrices on lamp shader
			modelLoc = glGetUniformLocation(lampShader.Program, "model");
			viewLoc = glGetUniformLocation(lampShader.Program, "view");
			projLoc = glGetUniformLocation(lampShader.Program, "projection");
			//set matrices
			glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
			glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
			glm::mat4 lightmodel;
			lightmodel = glm::mat4();
			lightmodel = glm::translate(lightmodel, lightPos);
			lightmodel = glm::scale(lightmodel, glm::vec3(0.2f));	//so lamp is a smaller cube!
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(lightmodel));

			//draw light object
			glBindVertexArray(lightVAO);
			glDrawArrays(GL_TRIANGLES, 0, 36);
			glBindVertexArray(0);

			//draw laser
			//laserShader.use();
			//glBindVertexArray(laserVAO);
			//std::cout << "Bind array" << glGetError() << std::endl;
			//glDrawArrays(GL_LINES, 0, 2);
			//std::cout << "Draw" << glGetError() << std::endl;

			if (mouseButton)
			{
				laserShader.use();
				glBindVertexArray(laserVAO);
				glDrawArrays(GL_TRIANGLES, 0, 3);
			}

			glfwSwapBuffers(window);
		}
	}
	glfwTerminate();
	logFile.close();
	return 0;
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);

	if (key >= 0 && key < 1024)
	{
		if (action == GLFW_PRESS)
			keys[key] = true;
		else if (action == GLFW_RELEASE)
			keys[key] = false;
	}
}

bool firstMouse = true;
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	GLfloat xoffset = xpos - lastX;
	GLfloat yoffset = lastY - ypos;  // Reversed since y-coordinates go from bottom to left

	lastX = xpos;
	lastY = ypos;

	camera.ProcessMouseMovement(xoffset, yoffset);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.ProcessMouseScroll(yoffset);
}

void click_callback(GLFWwindow* window, int button, int action, int mods, Shader &pickingShader)
{
	
}

void do_movement()
{
	// Camera controls
	if (keys[GLFW_KEY_W])
		camera.ProcessKeyboard(FORWARD, deltaTime);
	if (keys[GLFW_KEY_S])
		camera.ProcessKeyboard(BACKWARD, deltaTime);
	if (keys[GLFW_KEY_A])
		camera.ProcessKeyboard(LEFT, deltaTime);
	if (keys[GLFW_KEY_D])
		camera.ProcessKeyboard(RIGHT, deltaTime);
}

void drawHUD()
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glTranslatef(0, 0, 0);
	glOrtho(0.0, WIDTH, HEIGHT, 0.0, -1.0, 1.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glDisable(GL_CULL_FACE);

	glClear(GL_DEPTH_BUFFER_BIT);

	glBegin(GL_QUADS);
	glColor3f(255.0f, 255.0f, 255.0f);
	glVertex2f(0.0, 0.0);
	glVertex2f(10.0, 0.0);
	glVertex2f(10.0, 10.0);
	glVertex2f(0.0, 10.0);
	glEnd();

	//switch back to 3D rendering
	glMatrixMode(GL_PROJECTION);
	glMatrixMode(GL_MODELVIEW);
}

//learnopengl.com code
void loadFont()
{
	//load font
	FT_Library ft;
	if (FT_Init_FreeType(&ft))
		std::cout << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;
	FT_Face face;
	if (FT_New_Face(ft, "fonts/arial.ttf", 0, &face))
		std::cout << "ERROR::FREETYPE: Failed to load font" << std::endl;

	//define font size
	FT_Set_Pixel_Sizes(face, 0, 48);

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);	//textures can be multiple of 1 byte

	//loop all characters in ASCII set
	for (GLubyte c = 0; c < 128; c++)
	{
		//load character glyph
		if (FT_Load_Char(face, c, FT_LOAD_RENDER))
		{
			std::cout << "ERROR::FREETYPE: Failed to load glyph" << std::endl;
			continue;
		}
		//generate texture
		GLuint texture;
		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RED,
			face->glyph->bitmap.width,
			face->glyph->bitmap.rows,
			0, GL_RED, GL_UNSIGNED_BYTE,
			face->glyph->bitmap.buffer);
		//set texture options
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		//store character
		Character character = {
			texture,
			glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
			glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
			face->glyph->advance.x
		};
		charMap.insert(std::pair<GLchar, Character>(c, character));
	}
	FT_Done_Face(face);
	FT_Done_FreeType(ft);
}

void renderText(Shader &s, std::string text, GLfloat x, GLfloat y, GLfloat scale, glm::vec3 colour)
{
	//activate corresponding render state
	s.use();
	glUniform3f(glGetUniformLocation(s.Program, "textColour"), colour.x, colour.y, colour.z);
	glActiveTexture(GL_TEXTURE0);
	glBindVertexArray(fontVAO);

	//iterate through all characters
	std::string::const_iterator c;
	for (c = text.begin(); c != text.end(); c++)
	{
		Character ch = charMap[*c];

		GLfloat xpos = x + ch.bearing.x * scale;
		GLfloat ypos = y - (ch.size.y - ch.bearing.y) * scale;

		GLfloat w = ch.size.x * scale;
		GLfloat h = ch.size.y * scale;
		//update VBO for each character
		GLfloat vertices[6][4] = {
			{xpos, ypos + h, 0.0, 0.0},
			{xpos, ypos,	 0.0, 1.0},
			{xpos + w, ypos, 1.0, 1.0},
			{xpos, ypos + h, 0.0, 0.0},
			{xpos + w, ypos, 1.0, 1.0},
			{xpos + w, ypos + h, 1.0, 0.0}
		};
		//Render font texture over quad
		glBindTexture(GL_TEXTURE_2D, ch.textureID);
		//update content of VBO memory
		glBindBuffer(GL_ARRAY_BUFFER, fontVBO);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		//render quad
		glDrawArrays(GL_TRIANGLES, 0, 6);
		//now advance cursors for next glyph
		x += (ch.advance >> 6) * scale; //bitshift by 6 to get value in pixels
	}
	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void calculateMovement(Cube &current, int i)
{
	//calculate movement
	switch (waypoint)
	{
	case 1:
		//WAYPOINT 1
		if (current.cubePos.x > (cubeStartPos[i].x - -0.5f) && current.cubePos.z < (cubeStartPos[i].z + 1.0f))
		{
			current.cubePos.x -= 0.005f;
			current.cubePos.z += 0.01f;
		}
		else
		{
			waypoint = 2;
			logFile << "WAYPOINT 2" << std::endl;
		}
		break;
	case 2:
		//WAYPOINT 2
		if (current.cubePos.z < (cubeStartPos[i].z + 1.0f))
		{
			current.cubePos.z += 0.005f;
		}
		else
		{
			waypoint = 3;
			logFile << "WAYPOINT 3" << std::endl;
		}
		break;
	case 3:
		//WAYPOINT 3
		//SHOOT!!
		if (current.cubePos.z < (cubeStartPos[i].z + 3.0f))
		{
			current.cubePos.z += 0.005f;
		}
		else
		{
			//playerHealth -= 5;
			waypoint = 4;
			logFile << "WAYPOINT 4" << std::endl;
		}
		break;
	case 4:
		//waypoint 4
		if (current.cubePos.x > (cubeStartPos[i].x - 3.25f) && current.cubePos.z < (cubeStartPos[i].z + 7.0f))
		{
			current.cubePos.x -= 0.0025f;
			current.cubePos.z += 0.002f;
		}
		else
		{
			waypoint = 5;
			logFile << "WAYPOINT 5" << std::endl;
		}
		break;
	case 5:
		//waypoint 5
		if (current.cubePos.x > (cubeStartPos[i].x - 5.75f) && current.cubePos.z < (cubeStartPos[i].z + 14.0f))
		{
			current.cubePos.x -= 0.005f;
			current.cubePos.z += 0.001f;
		}
		else
		{
			waypoint = 6;
			logFile << "WAYPOINT 6" << std::endl;
		}
		break;
	case 6:
		//waypoint 6
		if (current.cubePos.x < cubeStartPos[i].x && current.cubePos.z < (cubeStartPos[i].z + 17.75f))
		{
			current.cubePos.x += 0.0005f;
			current.cubePos.z += 0.0025f;
		}
		else
		{
			waypoint = 7;
			logFile << "WAYPOINT 7" << std::endl;
		}
		break;
	case 7:
		//waypoint 7
		if (current.cubePos.x < (cubeStartPos[i].x + 7.4f) && current.cubePos.z >(cubeStartPos[i].z +8.2f))
		{
			current.cubePos.x += 0.0005f;
			current.cubePos.z -= 0.003f;
		}
		else
		{
			waypoint = 8;
			logFile << "WAYPOINT 8" << std::endl;
		}
		break;
	case 8:
		//waypoint 8
		if (current.cubePos.x < (cubeStartPos[i].x + 8.4f) && current.cubePos.z >(cubeStartPos[i].z - -4.0f))
		{
			current.cubePos.x += 0.0005f;
			current.cubePos.z -= 0.003f;
		}
		else
		{
			waypoint = 9;
			logFile << "WAYPOINT 9" << std::endl;
		}
		break;
	case 9:
		//waypoint 9
		if (current.cubePos.x < (cubeStartPos[i].x + 4.1f) && current.cubePos.z >(cubeStartPos[i].z - -2.5f))
		{
			current.cubePos.x += 0.0005f;
			current.cubePos.z -= 0.003f;
		}
		else
		{
			waypoint = 10;
			logFile << "WAYPOINT 10" << std::endl;
		}
		break;
	case 10:
		//waypoint 10
		if (current.cubePos.x > (cubeStartPos[i].x + 1.5f) && current.cubePos.z > cubeStartPos[i].z)
		{
			current.cubePos.x -= 0.0005f;
			current.cubePos.z -= 0.003f;
		}
		else
		{
			waypoint = 11;
			logFile << "WAYPOINT 11" << std::endl;
		}
		break;
	case 11:
		//back to start
		if (current.cubePos.x > cubeStartPos[i].x)
		{
			current.cubePos.x -= 0.0015f;
		}
		else
		{
			waypoint = 1;
			logFile << "WAYPOINT 1" << std::endl;
		}
		break;
	default:
		logFile << "MOVEMENT ERROR!" << std::endl;
	}
}
