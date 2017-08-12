#pragma warning(disable : 4996)
#include "Shader.h"

Shader::Shader(const GLchar* vertexPath, const GLchar* fragmentPath)
{
	//file for printing log output
	std::ofstream logFile;
	logFile.open("log.txt");
	std::time_t currentTime = std::time(nullptr);
	logFile << std::asctime(std::localtime(&currentTime));
	//retrieve vertex or fragment shader source code from filePath
	std::string vertexCode;
	std::string fragmentCode;
	std::ifstream vShaderFile;
	std::ifstream fShaderFile;
	//ensure ifstream objects can throw exceptions
	vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	try
	{
		//open files
		vShaderFile.open(vertexPath);
		fShaderFile.open(fragmentPath);
		std::stringstream vShaderStream, fShaderStream;
		//read file's buffer contents into streams
		vShaderStream << vShaderFile.rdbuf();
		fShaderStream << fShaderFile.rdbuf();
		//close file handlers
		vShaderFile.close();
		fShaderFile.close();
		//convert stream into GLchar array
		vertexCode = vShaderStream.str();
		fragmentCode = fShaderStream.str();
	}
	catch (std::ifstream::failure e)
	{
		logFile << "ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ" << std::endl;
	}
	const GLchar* vShaderCode = vertexCode.c_str();
	const GLchar* fShaderCode = fragmentCode.c_str();

	//compile shaders
	GLuint vertex, fragment;
	GLint success;
	GLchar infoLog[512];

	//vertex shader
	vertex = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex, 1, &vShaderCode, NULL);
	glCompileShader(vertex);
	//print compile errors if any
	glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(vertex, 512, NULL, infoLog);
		logFile << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
	}

	//same for fragment shader
	fragment = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment, 1, &fShaderCode, NULL);
	glCompileShader(fragment);
	glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(fragment, 512, NULL, infoLog);
		logFile << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
	}

	//shader program
	this->Program = glCreateProgram();
	glAttachShader(this->Program, vertex);
	glAttachShader(this->Program, fragment);
	glLinkProgram(this->Program);
	//print linking errors if any
	glGetProgramiv(this->Program, GL_LINK_STATUS, &success);
	if (!success)
	{
		glGetProgramInfoLog(this->Program, 512, NULL, infoLog);
		logFile << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
	}
	//delete shaders as they're linked into program and no longer necessary
	glDeleteShader(vertex);
	glDeleteShader(fragment);
	logFile.close();
}


Shader::Shader(const GLchar* vertexPath, const GLchar* fragmentPath, const GLchar* tessControlPath, const GLchar* tessEvalPath)
{
	//open logfile for writing debug data
	std::ofstream logFile;
	logFile.open("log.txt");
	std::time_t currentTime = std::time(nullptr);
	logFile << std::asctime(std::localtime(&currentTime));
	//retrieve vertex or fragment shader source code from filePath
	std::string vertexCode;
	std::string fragmentCode;
	std::string tcCode;
	std::string teCode;
	std::ifstream vShaderFile;
	std::ifstream fShaderFile;
	std::ifstream tcShaderFile;
	std::ifstream teShaderFile;
	//ensure ifstream objects can throw exceptions
	vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	teShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	tcShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	try
	{
		//open files
		vShaderFile.open(vertexPath);
		fShaderFile.open(fragmentPath);
		tcShaderFile.open(tessControlPath);
		teShaderFile.open(tessEvalPath);
		std::stringstream vShaderStream, fShaderStream, teShaderStream, tcShaderStream;
		//read file's buffer contents into streams
		vShaderStream << vShaderFile.rdbuf();
		fShaderStream << fShaderFile.rdbuf();
		teShaderStream << teShaderFile.rdbuf();
		tcShaderStream << tcShaderFile.rdbuf();
		//close file handlers
		vShaderFile.close();
		fShaderFile.close();
		tcShaderFile.close();
		teShaderFile.close();
		//convert stream into GLchar array
		vertexCode = vShaderStream.str();
		fragmentCode = fShaderStream.str();
		tcCode = tcShaderStream.str();
		teCode = teShaderStream.str();
	}
	catch (std::ifstream::failure e)
	{
		logFile << "ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ" << std::endl;
	}
	const GLchar* vShaderCode = vertexCode.c_str();
	const GLchar* fShaderCode = fragmentCode.c_str();
	const GLchar* tcShaderCode = tcCode.c_str();
	const GLchar* teShaderCode = teCode.c_str();

	//compile shaders
	GLuint vertex, fragment, tessCont, tessEval;
	GLint success;
	GLchar infoLog[512];

	//vertex shader
	vertex = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex, 1, &vShaderCode, NULL);
	glCompileShader(vertex);
	//print compile errors if any
	glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(vertex, 512, NULL, infoLog);
		logFile << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
	}

	//same for fragment shader
	fragment = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment, 1, &fShaderCode, NULL);
	glCompileShader(fragment);
	glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(fragment, 512, NULL, infoLog);
		logFile << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
	}

	//same for tessellation control
	tessCont = glCreateShader(GL_TESS_CONTROL_SHADER);
	glShaderSource(tessCont, 1, &tcShaderCode, NULL);
	glCompileShader(tessCont);
	glGetShaderiv(tessCont, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(tessCont, 512, NULL, infoLog);
		logFile << "ERROR::SHADER::TESS_CONTROL::COMPILATION_FAILED\n" << infoLog << std::endl;
	}

	//same for tessellation evaluation
	tessEval = glCreateShader(GL_TESS_EVALUATION_SHADER);
	glShaderSource(tessEval, 1, &teShaderCode, NULL);
	glCompileShader(tessEval);
	glGetShaderiv(tessEval, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(tessEval, 512, NULL, infoLog);
		logFile << "ERROR::SHADER::TESS_EVALUATION::COMPILATION_FAILED\n" << infoLog << std::endl;
	}

	//shader program
	this->Program = glCreateProgram();
	glAttachShader(this->Program, vertex);
	glAttachShader(this->Program, fragment);
	glLinkProgram(this->Program);
	//print linking errors if any
	glGetProgramiv(this->Program, GL_LINK_STATUS, &success);
	if (!success)
	{
		glGetProgramInfoLog(this->Program, 512, NULL, infoLog);
		logFile << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
	}
	//delete shaders as they're linked into program and no longer necessary
	glDeleteShader(vertex);
	glDeleteShader(fragment);
}

void Shader::use() { glUseProgram(this->Program); }
