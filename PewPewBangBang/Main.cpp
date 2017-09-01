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
//SOIL
#include <SOIL.h>
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
void click_callback(GLFWwindow* window, int button, int action, int mods);
void do_movement();
void drawHUD();
void loadFont();
void renderText(Shader &s, std::string text, GLfloat x, GLfloat y, GLfloat scale, glm::vec3 colour);
void gameOver();
void calculateMovement(Cube &current, int i);
void drawLaser();
void drawCrosshair(Shader &shader, GLuint VAO);

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
const glm::vec3 cubeStartPos[] = {
	glm::vec3(0.0f, 0.0f, -15.0f),
	glm::vec3(-2.0f, 0.0f, -17.0f),
	glm::vec3(4.0f, -3.0f, -20.0f)
};

Cube cubeArray[] = {
	Cube(cubeStartPos[0], glm::vec3(0.0f, 1.0f, 0.0f)),	//green
	Cube(cubeStartPos[1], glm::vec3(0.0f, 0.0f, 1.0f)),	//blue
	Cube(cubeStartPos[2], glm::vec3(1.0f, 0.0f, 0.0f))	//red
};

//open log file for debugging output
std::ofstream logFile;

//movement
int score = 0;
 
int playerHealth = 100;

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
	Shader lightShader("Shaders/light.vs", "Shaders/light.frag");
	Shader lampShader("Shaders/lamp.vs", "Shaders/lamp.frag");
	Shader fontShader("Shaders/font.vs", "Shaders/font.frag");
	Shader pickingShader("Shaders/picking.vs", "Shaders/picking.frag");
	Shader particleShader("Shaders/particle.vs", "Shaders/particle.frag");
	Shader triangleShader("Shaders/triangle.vs", "Shaders/triangle.frag");

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
		// Positions           // Normals           // Texture Coords
		-0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f,
		0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f,
		0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f,
		0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f,
		-0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f,

		-0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
		0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f,
		0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,
		0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,
		-0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f,
		-0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,

		-0.5f, 0.5f, 0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
		-0.5f, 0.5f, -0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f,
		-0.5f, -0.5f, -0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
		-0.5f, -0.5f, 0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
		-0.5f, 0.5f, 0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f,

		0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
		0.5f, 0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f,
		0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
		0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
		0.5f, -0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
		0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,

		-0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f,
		0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 1.0f,
		0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f,
		0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f,
		-0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f,
		-0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f,

		-0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
		0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f,
		0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
		0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
		-0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f,
		-0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f
	};

	GLfloat laserVerts[] = {
		// positions         // colors
		0.03f, -1.0f, 0.0f, 1.0f, 0.0f, 0.0f,   // bottom right
		-0.03f, -1.0f, 0.0f, 1.0f, 0.0f, 0.0f,   // bottom left
		0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f    // top 
	};

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

	//open logfile
	logFile.open("debuglog.txt", std::ofstream::app);

	GLuint VBO, containerVAO;
	glGenVertexArrays(1, &containerVAO);
	glGenBuffers(1, &VBO);
	glBindVertexArray(containerVAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	//position attributes of cube
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);
	//normal attributes
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);
	//texture attributes
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
	glEnableVertexAttribArray(2);
	glBindVertexArray(0);

	GLuint cubetexture;
	GLuint specMap;
	glGenTextures(1, &cubetexture);
	int width, height;
	unsigned char* image = SOIL_load_image("media/cube.png", &width, &height, 0, SOIL_LOAD_RGB);
	glBindTexture(GL_TEXTURE_2D, cubetexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
	glGenerateMipmap(GL_TEXTURE_2D);
	SOIL_free_image_data(image);

	//set parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST_MIPMAP_NEAREST);
	glBindTexture(GL_TEXTURE_2D, 0);

	//specular map
	glGenTextures(1, &specMap);
	glBindTexture(GL_TEXTURE_2D, specMap);
	image = SOIL_load_image("media/cube_specular.png", &width, &height, 0, SOIL_LOAD_RGB);
	
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
	glGenerateMipmap(GL_TEXTURE_2D);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST_MIPMAP_NEAREST);
	SOIL_free_image_data(image);
	glBindTexture(GL_TEXTURE_2D, 0);

	GLuint lightVAO;

	glGenVertexArrays(1, &lightVAO);
	glBindVertexArray(lightVAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	//lamp - same shape as the other cube!
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GL_FLOAT), (GLvoid*)0);
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

	//crosshair
	GLuint crosshair1VAO, crosshair1VBO;
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
	GLuint crosshair2VAO, crosshair2VBO;
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
	GLuint crosshair3VAO, crosshair3VBO;
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

	lightShader.use();
	glUniform1i(glGetUniformLocation(lightShader.Program, "material.diffuse"), 0);
	glUniform1i(glGetUniformLocation(lightShader.Program, "material.specular"), 1);

	lampShader.use();
	glUniform1i(glGetUniformLocation(lampShader.Program, "ourTexture"), 0);

	SoundEngine->play2D("media/ambientloop.mp3", GL_TRUE);

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
			if (!cubeArray[i].alive && (glfwGetTime() - cubeArray[i].timeKilled) >= 3.0)
			{
				cubeArray[i].alive = true;
				//reset position
				cubeArray[i].cubePos = cubeStartPos[i];
				cubeArray[i].waypoint = 1;
			}

			if (cubeArray[i].alive)	//only do them if they're alive or respawn them
			{
				//calculate movement - they should only move and shoot if they're alive!
				calculateMovement(cubeArray[i], i);

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
			SoundEngine->play2D("media/phasers.wav", GL_FALSE);

			//read the pixel at the centre of the screen
			unsigned char data[4];
			glReadPixels(WIDTH / 2, HEIGHT / 2, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, data);

			int pickedID =
				data[0] +
				data[1] * 256 +
				data[2] * 256 * 256;

			std::cout << pickedID << std::endl;

			if (pickedID == 0x0000ff00)
			{
				cubeArray[0].alive = false;
				cubeArray[0].timeKilled = glfwGetTime();
				SoundEngine->play2D("media/boing.wav", GL_FALSE);
				score += 100;
			}
			else if (pickedID == 0x00ff0000)
			{
				cubeArray[1].alive = false;
				cubeArray[1].timeKilled = glfwGetTime();
				SoundEngine->play2D("media/boing.wav", GL_FALSE);
				score += 100;
			}
			else if (pickedID == 0x000000ff)
			{
				cubeArray[2].alive = false;
				cubeArray[2].timeKilled = glfwGetTime();
				SoundEngine->play2D("media/boing.wav", GL_FALSE);
				score += 100;
			}
		}
		
		//END PICKING
		
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		std::ostringstream scoreText;
		scoreText << "Score: " << score;

		//check if player is dead
		if (playerHealth <= 0)
		{
			glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
			renderText(fontShader, "Game Over!", 30.0f, 300.0f, 1.5f, glm::vec3(1.0f, 0.0f, 0.0f));
			renderText(fontShader, "Press Esc to Exit", 30.0f, 100.0f, 1.0f, glm::vec3(1.0f, 0.0f, 0.0f));
			renderText(fontShader, scoreText.str(), 30.0f, 20.0f, 0.7f, glm::vec3(1.0f, 0.0f, 0.0f));
			glfwSwapBuffers(window);
		}
		else
		{
			//do_movement();

			GLfloat currentFrame = glfwGetTime();
			deltaTime = currentFrame - lastFrame;
			lastFrame = currentFrame;

			std::ostringstream healthText;
			healthText << "Health: " << playerHealth;

			renderText(fontShader, healthText.str(), 25.0f, 25.0f, 1.0f, glm::vec3(0.5f, 0.8f, 0.2f));
			renderText(fontShader, scoreText.str(), 540.0f, 570.0f, 0.7f, glm::vec3(0.3f, 0.7f, 0.9f));
		
			//set uniforms and draw objects
			lightShader.use();
			GLint matSpecularLoc = glGetUniformLocation(lightShader.Program, "material.specular");
			GLint matShineLoc = glGetUniformLocation(lightShader.Program, "material.shininess");
			GLint lightAmbientLoc = glGetUniformLocation(lightShader.Program, "light.ambient");
			GLint lightDiffuseLoc = glGetUniformLocation(lightShader.Program, "light.diffuse");
			GLint lightSpecularLoc = glGetUniformLocation(lightShader.Program, "light.specular");
			GLint lightPosLoc = glGetUniformLocation(lightShader.Program, "lightPos");
			GLint viewPosLoc = glGetUniformLocation(lightShader.Program, "viewPos");
			glUniform3f(matSpecularLoc, 0.5f, 0.5f, 0.5f);
			glUniform3f(lightAmbientLoc, 0.4f, 0.4f, 0.4f);
			glUniform3f(lightDiffuseLoc, 0.7f, 0.7f, 0.7f);
			glUniform3f(lightSpecularLoc, 1.0f, 1.0f, 1.0f);
			glUniform1f(matShineLoc, 75.0f);
			glUniform3f(viewPosLoc, camera.Position.x, camera.Position.y, camera.Position.z);
			glUniform3f(lightPosLoc, lightPos.x, lightPos.y, lightPos.z);


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
					glActiveTexture(GL_TEXTURE0);
					glBindTexture(GL_TEXTURE_2D, cubetexture);
					glActiveTexture(GL_TEXTURE1);
					glBindTexture(GL_TEXTURE_2D, specMap);

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

			//draw crosshair
			triangleShader.use();
			glBindVertexArray(crosshair1VAO);
			glDrawArrays(GL_TRIANGLES, 0, 3);

			triangleShader.use();
			glBindVertexArray(crosshair2VAO);
			glDrawArrays(GL_TRIANGLES, 0, 3);

			triangleShader.use();
			glBindVertexArray(crosshair3VAO);
			glDrawArrays(GL_TRIANGLES, 0, 3);

			if (mouseButton)
			{
				triangleShader.use();
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

//void do_movement()
//{
//	// Camera controls
//	if (keys[GLFW_KEY_W])
//		camera.ProcessKeyboard(FORWARD, deltaTime);
//	if (keys[GLFW_KEY_S])
//		camera.ProcessKeyboard(BACKWARD, deltaTime);
//	if (keys[GLFW_KEY_A])
//		camera.ProcessKeyboard(LEFT, deltaTime);
//	if (keys[GLFW_KEY_D])
//		camera.ProcessKeyboard(RIGHT, deltaTime);
//}

//learnopengl.com code
void loadFont()
{
	//load font
	FT_Library ft;
	if (FT_Init_FreeType(&ft))
		std::cout << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;
	FT_Face face;
	if (FT_New_Face(ft, "fonts/segoeui.ttf", 0, &face))
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
	//find cube by ID
	switch (i)
	{
	case 0:
		switch (current.waypoint)
		{
		case 1:
			//WAYPOINT 1
			if (current.cubePos.x > -4.0f && current.cubePos.z < -7.0f)
			{
				//z needs to be double x
				current.cubePos.x -= 0.005f;
				current.cubePos.z += 0.01f;
			}
			else
			{
				current.waypoint = 2;
				logFile << "CUBE 1 WAYPOINT 2" << std::endl;
				SoundEngine->play2D("media/jet.wav", GL_FALSE);
			}
			break;
		case 2:
			//WAYPOINT 2
			if (current.cubePos.x < -2.0f && current.cubePos.z < 13.0f)
			{
				//z is ten times x
				current.cubePos.x += 0.0025f;
				current.cubePos.z += 0.025f;
			}
			else
			{
				//shoot!
				current.waypoint = 3;
				playerHealth -= 5;
				SoundEngine->play2D("media/laser.wav", GL_FALSE);
				logFile << "CUBE 1 WAYPOINT 3" << std::endl;
			}
			break;
		case 3:
			//waypoint 4
			if (current.cubePos.x < 2.0f)
			{
				//x moves +4
				current.cubePos.x += 0.005f;
			}
			else
			{
				current.waypoint = 4;
				logFile << "CUBE 1 WAYPOINT 4" << std::endl;
			}
			break;
		case 4:
			//waypoint 5
			if (current.cubePos.x < 6.0f && current.cubePos.z > -3.0f)
			{
				//z should be x*4
				current.cubePos.x += 0.01f;
				current.cubePos.z -= 0.04f;
			}
			else
			{
				current.waypoint = 5;
				logFile << "CUBE 1 WAYPOINT 5" << std::endl;
			}
			break;
		case 5:
			//waypoint 6
			if (current.cubePos.x > cubeStartPos[i].x && current.cubePos.z > cubeStartPos[i].z)
			{
				//z is double x
				current.cubePos.x -= 0.004f;
				current.cubePos.z -= 0.008f;
			}
			else
			{
				current.waypoint = 1;
				logFile << "CUBE 1 START" << std::endl;
				SoundEngine->play2D("media/jet_flyby1.wav", GL_FALSE);
			}
			break;
		default:
			logFile << "CUBE 1 MOVEMENT ERROR!" << std::endl;
		};
		break;
		
	case 1:
		switch (current.waypoint)
		{
		case 1:
			//WAYPOINT 1
			if (current.cubePos.z < -7.0f)
			{
				//z is 5 times x
				current.cubePos.z += 0.01f;
			}
			else
			{
				current.waypoint = 2;
				logFile << "CUBE 2 WAYPOINT 2" << std::endl;
			}
			break;
		case 2:
			//WAYPOINT 2
			//SHOOT!!
			if (current.cubePos.y < 4.0f && current.cubePos.z < 1.0f)
			{
				//y+4 z+8 :: z is double y
				current.cubePos.y += 0.005f;
				current.cubePos.z += 0.01f;
			}
			else
			{
				playerHealth -= 5;
				SoundEngine->play2D("media/laser.wav", GL_FALSE);
				current.waypoint = 3;
				logFile << "CUBE 2 WAYPOINT 3" << std::endl;
				
			}
			break;
		case 3:
			//waypoint 3
			if (current.cubePos.y > 0.0f && current.cubePos.z < 13.0f)
			{
				//y - 4, z + 12 :: z = y*-4
				current.cubePos.y -= 0.004f;
				current.cubePos.z += 0.016f;
			}
			else
			{
				current.waypoint = 4;
				logFile << "CUBE 2 WAYPOINT 4" << std::endl;
			}
			break;
		case 4:
			//waypoint 5
			if (current.cubePos.y > -5.0f && current.cubePos.z > 3.0f)
			{
				//y - 5, z - 10 :: z = y*2
				current.cubePos.y -= 0.01f;
				current.cubePos.z -= 0.02f;
			}
			else
			{
				current.waypoint = 5;
				logFile << "CUBE 2 WAYPOINT 5" << std::endl;
			}
			break;
		case 5:
			//waypoint 6
			//BACK TO START
			if (current.cubePos.y < 0.0f && current.cubePos.z > -17.0f)
			{
				//x-2, y+5, z-20
				current.cubePos.y += 0.005f;
				current.cubePos.z -= 0.02f;
			}
			else
			{
				current.waypoint = 1;
				logFile << "CUBE 2 START" << std::endl;
				SoundEngine->play2D("media/jet_flyby1.wav", GL_FALSE);
			}
			break;
		default:
			logFile << "CUBE 2 MOVEMENT ERROR!" << std::endl;
		}
		break;
	case 2:
		switch (current.waypoint)
		{
		case 1:
			//WAYPOINT 1
			if (current.cubePos.z < -8.0f)
			{
				current.cubePos.z += 0.01f;
			}
			else
			{
				current.waypoint = 2;
				logFile << "CUBE 3 WAYPOINT 2" << std::endl;
			}
			break;
		case 2:
			//WAYPOINT 2
			if (current.cubePos.x > 1.0f && current.cubePos.z < 19.0f)
			{
				//x - 3, z + 27 :: z = x * 9
				current.cubePos.x -= 0.001f;
				current.cubePos.z += 0.009f;
			}
			else
			{
				current.waypoint = 3;
				//SHOOT
				playerHealth -= 5;
				logFile << "CUBE 3 WAYPOINT 3" << std::endl;
				SoundEngine->play2D("media/laser.wav", GL_FALSE);
			}
			break;
		case 3:
			//waypoint 4
			if (current.cubePos.x > -5.0f && current.cubePos.z > 3.0f)
			{
				//x+4, Z-16 :: z = x * -4
				current.cubePos.x -= 0.002f;
				current.cubePos.z -= 0.004f;
			}
			else
			{
				current.waypoint = 4;
				logFile << "CUBE 3 WAYPOINT 4" << std::endl;
			}
			break;
		case 4:
			//waypoint 5
			if (current.cubePos.x < 4.0f && current.cubePos.z > -20.0f)
			{
				//x-1, z-23
				current.cubePos.x += 0.0022f;
				current.cubePos.z -= 0.0046f;
			}
			else
			{
				current.waypoint = 1;
				logFile << "CUBE 3 START" << std::endl;
				SoundEngine->play2D("media/jet_flyby1.wav", GL_FALSE);
			}
			break;
		default:
			//logFile << "CUBE 3 MOVEMENT ERROR!" << std::endl;
			break;
		}
		break;
	default:
		//throw some kind of exception
		logFile << "INVALID CUBE" << std::endl;
		break;
	}

}
