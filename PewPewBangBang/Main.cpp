#define GLEW_STATIC
#include <glew\glew.h>
#include <GLFW\glfw3.h>
#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <glm\gtc\type_ptr.hpp>
#include "Camera.h"
#include "Shader.h"
#include <iostream>
#include <fstream>

// Function prototypes
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void do_movement();

// Window dimensions
const GLuint WIDTH = 800, HEIGHT = 600;

bool keys[1024];
GLfloat deltaTime = 0.0f;	//time between current frame and last frame
GLfloat lastFrame = 0.0f;	//time of last frame

Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
GLfloat lastX = WIDTH / 2.0;
GLfloat lastY = HEIGHT / 2.0;

//light vector
glm::vec3 lightPos(1.2f, 1.0f, 2.0f);
//cube vector
glm::vec3 cubePos(0.0f, 0.0f, -7.2f);

//movement
int waypoint = 1;

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
	
	//DEBUG - enable debugging
	glEnable(GL_DEBUG_OUTPUT);

	Shader lightShader("light.vs", "light.frag");
	Shader lampShader("lamp.vs", "lamp.frag");
	camera.MouseSensitivity = 0.05f;

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

	//open log file for debugging output
	std::ofstream logFile;
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
	//game loop
	while (!glfwWindowShouldClose(window))
	{
		glfwPollEvents();
		do_movement();

		GLfloat currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		//calculate movement
	/*	if (forward)
		{
			if (movement < 1000)
			{
				cubePos.y += 0.001f;
				movement++;
				logFile << "Movement forward -- CUBE Y: " << cubePos.y << "  MOVEMENT VAL:  " << movement << std::endl;
			}
			else if (movement >= 1000 || cubePos.y >= 1.0f)
			{
				forward = false;
				logFile << "Make false " << std::endl;
			}
		}
		else if (!forward)
		{
			if (movement > 0)
			{
				cubePos.y -= 0.001f;
				movement--;
				logFile << "Movement downward -- CUBE Y: " << cubePos.y << "  MOVEMENT VAL:  " << movement << std::endl;
			}
			else if (movement <= 0 || cubePos.y <= 0.0f)
			{
				forward = true;
				logFile << "Make true " << std::endl;
			}
		}*/

		switch (waypoint)
		{
			case 1:
				//WAYPOINT 1
				if (cubePos.x > -2.4f || cubePos.z < 7.2f)
				{
					cubePos.x -= 0.0005f;
					cubePos.z += 0.003f;
				}
				else
				{
					waypoint = 2;
					logFile << "WAYPOINT 2" << std::endl;
				}
				break;
			case 2:
				//WAYPOINT 2
				if (cubePos.x < 0.0f || cubePos.z < 9.6f)
				{
					cubePos.x += 0.001f;
					cubePos.z += 0.001f;
				}
				else
				{
					waypoint = 3;
					logFile << "WAYPOINT 3" << std::endl;
				}
				break;
			case 3:
				//WAYPOINT 3
				if (cubePos.x < 2.4f || cubePos.z > 7.2f)
				{
					cubePos.x += 0.001f;
					cubePos.z -= 0.001f;
				}
				else
				{
					waypoint = 4;
					logFile << "WAYPOINT 4" << std::endl;
				}
				break;
			case 4:
				//BACK TO START
				if (cubePos.x > 0.0f || cubePos.z > -7.2f)
				{
					cubePos.x -= 0.0005f;
					cubePos.z -= 0.003f;
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
		

		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

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

		//draw container
		glBindVertexArray(containerVAO);
		glm::mat4 cubemodel;
		cubemodel = glm::mat4();
		cubemodel = glm::translate(cubemodel, cubePos);
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(cubemodel));
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glBindVertexArray(0);

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

		glfwSwapBuffers(window);
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
