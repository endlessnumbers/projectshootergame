#define GLEW_STATIC
#include <glew\glew.h>
#include <GLFW\glfw3.h>
//GLM
#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <glm\gtc\type_ptr.hpp>
//IRRKLANG
#include <irrklang\irrKlang.h>
//SOIL
#include <SOIL.h>
//LOCAL
#include "Camera.h"
#include "Shader.h"
#include "Cube.h"
#include "Font.h"
#include "Crosshair.h"
#include "Laser.h"
#include "Stars.h"
#include "Vertices.h"
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

GLfloat deltaTime = 0.0f;	//time between current frame and last frame
GLfloat lastFrame = 0.0f;	//time of last frame

bool keys[1024];
bool firstClick = false;

//create camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));

//light vector
glm::vec3 lightPos(1.2f, 1.0f, 2.0f);

const GLuint WIDTH = 1024, HEIGHT = 800;

GLfloat lastX = WIDTH / 2.0;
GLfloat lastY = HEIGHT / 2.0;

const glm::vec3 cubeStartPos[] = {
	glm::vec3(0.0f, 0.0f, -15.0f),
	glm::vec3(-2.0f, 0.0f, -17.0f),
	glm::vec3(4.0f, -3.0f, -20.0f)
};

Cube cubeArray[] = {
	Cube(cubeStartPos[0], glm::vec3(0.0f, 1.0f, 0.0f), 0),	//green
	Cube(cubeStartPos[1], glm::vec3(0.0f, 0.0f, 1.0f), 1),	//blue
	Cube(cubeStartPos[2], glm::vec3(1.0f, 0.0f, 0.0f), 2)	//red
};

Bullet bulletArray[] = {
	Bullet(0, cubeArray[0].cubePos),
	Bullet(1, cubeArray[1].cubePos),
	Bullet(2, cubeArray[2].cubePos)
};

Stars stars;

ArrayContainer arrays;

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

	GLfloat starvertices[] = {
		(0.0f, 0.0f, -21.0f),
	};

	//setup shaders
	Shader lightShader("Shaders/light.vert", "Shaders/light.frag");
	Shader lampShader("Shaders/lamp.vert", "Shaders/lamp.frag");
	Shader pickingShader("Shaders/picking.vert", "Shaders/picking.frag");
	Shader fontShader("Shaders/font.vert", "Shaders/font.frag");
	Shader particleShader("Shaders/particle.vert", "Shaders/particle.frag");
	Shader triangleShader("Shaders/triangle.vert", "Shaders/triangle.frag");
	Shader bulletShader("Shaders/bullet.vert", "Shaders/bullet.frag");
	Shader starShader("Shaders/stars.vert", "Shaders/stars.frag");

	camera.MouseSensitivity = 0.05f;

	//prepare font shaders
	glm::mat4 fontProjection = glm::ortho(0.0f, static_cast<GLfloat>(WIDTH), 0.0f,
		static_cast<GLfloat>(HEIGHT));
	fontShader.use();
	glUniformMatrix4fv(glGetUniformLocation(fontShader.Program, "projection"), 1, GL_FALSE,
		glm::value_ptr(fontProjection));

	//PLAYER
	Player player;
	//FONT
	Font uiFont;
	//CROSSHAIR
	Crosshair crosshair;
	//LASER
	Laser laser;

	GLuint VBO, containerVAO;
	glGenVertexArrays(1, &containerVAO);
	glGenBuffers(1, &VBO);
	glBindVertexArray(containerVAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(arrays.vertices), arrays.vertices, GL_STATIC_DRAW);
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
	unsigned char* image = SOIL_load_image("media/cubetexture.png", &width, &height, 0, SOIL_LOAD_RGB);
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

	GLuint bulletVAO;
	GLuint bulletVBO;
	glGenVertexArrays(1, &bulletVAO);
	glGenBuffers(1, &bulletVBO);
	glBindVertexArray(bulletVAO);
	glBindBuffer(GL_ARRAY_BUFFER, bulletVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(arrays.bulletVertices), arrays.bulletVertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0);
	glEnableVertexAttribArray(0);
	glBindVertexArray(0);

	//STARS
	GLuint starvao, starvbo;
	glGenVertexArrays(1, &starvao);
	glGenBuffers(1, &starvbo);
	glBindVertexArray(starvao);
	glBindBuffer(GL_ARRAY_BUFFER, starvbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(starvertices), starvertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0);
	glEnableVertexAttribArray(0);
	glBindVertexArray(0);

	lightShader.use();
	glUniform1i(glGetUniformLocation(lightShader.Program, "material.diffuse"), 0);
	glUniform1i(glGetUniformLocation(lightShader.Program, "material.specular"), 1);

	SoundEngine->play2D("media/ambientloop.mp3", GL_TRUE);

	//game loop
	while (!glfwWindowShouldClose(window))
	{
		glfwPollEvents();

		glfwSetMouseButtonCallback(window, GLFW_MOUSE_BUTTON_1);
		

		//INTRO PAGE
		while (!firstClick)
		{
			glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			uiFont.renderText(fontShader, "Welcome", 200.0f, 700.0f, 1.5f, glm::vec3(1.0f, 0.0f, 0.0f));
			uiFont.renderText(fontShader, "You are a law-abiding citizen who was flying their spaceship", 10.0f, 600.0f, 0.8f, glm::vec3(1.0f, 0.0f, 0.0f));
			uiFont.renderText(fontShader, "while running some errands one day. Suddenly, you are", 10.0f, 550.0f, 0.8f, glm::vec3(1.0f, 0.0f, 0.0f));
			uiFont.renderText(fontShader, "attacked by a gang of yobos in high-tech jet-powered cubes,", 10.0f, 500.0f, 0.8f, glm::vec3(1.0f, 0.0f, 0.0f));
			uiFont.renderText(fontShader, "who want to steal the fancy M&S meals you've just bought.", 10.0f, 450.0f, 0.8f, glm::vec3(1.0f, 0.0f, 0.0f));
			uiFont.renderText(fontShader, "Make your valiant last stand here. Fight for your fancy mash!", 10.0f, 400.0f, 0.8f, glm::vec3(1.0f, 0.0f, 0.0f));
			uiFont.renderText(fontShader, "Use the mouse to aim and left mouse to shoot", 10.0f, 300.0f, 0.8f, glm::vec3(1.0f, 0.0f, 0.0f));
			uiFont.renderText(fontShader, "Click anywhere to begin", 100.0f, 200.0f, 1.0f, glm::vec3(1.0f, 0.0f, 0.0f));

			glfwSwapBuffers(window);

			glfwPollEvents();
			bool startButton = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT);

			if (startButton)
			{
				firstClick = true;
				break;
			}
		}

		//PICKING

		//clear screen in white, for picking
		glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		pickingShader.use();

		for (int i = 0; i < 3; i++)
		{
			//respawn cubes
			if (!cubeArray[i].alive && (glfwGetTime() - cubeArray[i].timeKilled) >= 2.0)
			{
				cubeArray[i].alive = true;
				//reset position
				cubeArray[i].cubePos = cubeStartPos[i];
				cubeArray[i].waypoint = 1;
			}

			if (cubeArray[i].alive)	//only do them if they're alive or respawn them
			{
				//calculate movement - they should only move and shoot if they're alive!
				cubeArray[i].calculateMovement(player, bulletArray);

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

		bool mouseButton = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT);

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

			if (pickedID == 0x0000ff00)
			{
				cubeArray[0].alive = false;
				cubeArray[0].timeKilled = glfwGetTime();
				SoundEngine->play2D("media/boing.wav", GL_FALSE);
				player.increaseScore();
			}
			else if (pickedID == 0x00ff0000)
			{
				cubeArray[1].alive = false;
				cubeArray[1].timeKilled = glfwGetTime();
				SoundEngine->play2D("media/boing.wav", GL_FALSE);
				player.increaseScore();
			}
			else if (pickedID == 0x000000ff)
			{
				cubeArray[2].alive = false;
				cubeArray[2].timeKilled = glfwGetTime();
				SoundEngine->play2D("media/boing.wav", GL_FALSE);
				player.increaseScore();
			}
		}

		//END PICKING

		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		std::ostringstream scoreText;
		scoreText << "Score: " << player.getScore();

		//check if player is dead
		if (player.getHealth() <= 0)
		{
			glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
			uiFont.renderText(fontShader, "Game Over!", 30.0f, 300.0f, 1.5f, glm::vec3(1.0f, 0.0f, 0.0f));
			uiFont.renderText(fontShader, "Press Esc to Exit", 30.0f, 100.0f, 1.0f, glm::vec3(1.0f, 0.0f, 0.0f));
			uiFont.renderText(fontShader, scoreText.str(), 30.0f, 20.0f, 0.7f, glm::vec3(1.0f, 0.0f, 0.0f));
		}
		else
		{
			GLfloat currentFrame = glfwGetTime();
			deltaTime = currentFrame - lastFrame;
			lastFrame = currentFrame;

			std::ostringstream healthText;
			healthText << "Health: " << player.getHealth();
			
			uiFont.renderText(fontShader, healthText.str(), 25.0f, 25.0f, 1.0f, glm::vec3(0.5f, 0.8f, 0.2f));
			uiFont.renderText(fontShader, scoreText.str(), 800.0f, 750.0f, 0.7f, glm::vec3(0.3f, 0.7f, 0.9f));

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

			//bullet
			bulletShader.use();
			modelLoc = glGetUniformLocation(bulletShader.Program, "model");
			viewLoc = glGetUniformLocation(bulletShader.Program, "view");
			projLoc = glGetUniformLocation(bulletShader.Program, "projection");
			//set matrices
			glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
			glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

			for (int i = 0; i < 3; i++)
			{
				if (cubeArray[i].alive)
				{
					lightShader.use();
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

					if (bulletArray[i].alive)
					{
						bulletArray[i].calculateMovement();

						bulletShader.use();
						glBindVertexArray(bulletVAO);
						glm::mat4 bulletmodel;
						bulletmodel = glm::mat4();
						bulletmodel = glm::translate(bulletmodel, bulletArray[i].bulletPos);
						glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(bulletmodel));

						//draw bullet object
						glDrawArrays(GL_TRIANGLES, 0, 36);
						glBindVertexArray(0);
					}
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

			crosshair.draw(triangleShader);

			for (int i = 0; i < sizeof(arrays.starpos); i++)
			{
				//stars
				starShader.use();
				modelLoc = glGetUniformLocation(bulletShader.Program, "model");
				viewLoc = glGetUniformLocation(bulletShader.Program, "view");
				projLoc = glGetUniformLocation(bulletShader.Program, "projection");
				glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
				glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
				glm::mat4 starmodel;
				starmodel = glm::mat4();

				starmodel = glm::translate(starmodel, arrays.starpos[i]);
				glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(starmodel));
				glBindVertexArray(starvao);
				glDrawArrays(GL_POINTS, 0, 1);
				glBindVertexArray(0);
			}

			if (mouseButton)
			{
				laser.draw(triangleShader);
			}

			glfwSwapBuffers(window);
		}
	}
	glfwTerminate();
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