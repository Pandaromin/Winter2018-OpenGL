// CleanGL.cpp : Defines the entry point for the console application.
//

/*

Vector to VBO
VAO tells what VBO does

*/
#include "stdafx.h"
#include <vector>
#include <glew\glew.h>
#include <GLFW\glfw3.h>
#include <glm\glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <fstream>
#include <string>



const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 800;
GLFWwindow* window;

float xrad = 0.0f;
float yrad =0.0f;
float zrad = 0.0f; //currently used for zooming. can it be used l;ater?


void fb_size_callback(GLFWwindow* window, int WINDOW_WIDTH, int WINDOW_HEIGHT);
GLuint loadShaders(std::string vertex_shader_path, std::string fragment_shader_path);
void processInput(GLFWwindow *window);
int init() {

	glfwInit();
	glEnable(GL_DEPTH_TEST);
	
	window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Assignment 1", nullptr, nullptr);
	if (window == nullptr) {
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);
	if (glewInit() != 0) {
		return -1;
	}

	//glfwSetFramebufferSizeCallback(window, fb_size_callback);

	return 0;
}

int main()
{

	if (init() != 0) {
		return -1;
	}
	GLuint shdr = loadShaders("v.s", "f.s");

	glUseProgram(shdr);
	std::vector<glm::vec3> tile = {
		glm::vec3(-0.5,0,-0.5), //bottom left
		glm::vec3(0.5,0,-0.5), //bottom right
		glm::vec3(0.5,0,0.5), //upper right
		glm::vec3(-0.5,0,0.5) //upper left
	};
	//XYZ axes 
	std::vector<glm::vec3> coordaxes{
		glm::vec3(0.0f,0.0f,0.0f),glm::vec3(5.0f,0.0f,0.0f), //X

		glm::vec3(0.0f,0.0f,0.0f),glm::vec3(0.0f,5.0f,0.0f), //Y

		glm::vec3(0.0f,0.0f,0.0f),glm::vec3(0.0f,0.0f,5.0f) //Z

	};

	GLuint VAO;
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	GLuint VBO;
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3)*tile.size(), &tile[0], GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0 * sizeof(float), 0);
	glEnableVertexAttribArray(0);

	GLuint axisVAO;
	glGenVertexArrays(1, &axisVAO);
	glBindVertexArray(axisVAO);

	GLuint axisVBO;
	glGenBuffers(1, &axisVBO);
	glBindBuffer(GL_ARRAY_BUFFER, axisVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3)*coordaxes.size(), &coordaxes[0], GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);


	//Cube
	std::vector<glm::vec3> unitCube{
		//front face
		glm::vec3(-0.5f,-0.5f,0.5f),glm::vec3(0.5f,-0.5f,0.5f),
		glm::vec3(0.5f,0.5f,0.5f),glm::vec3(-0.5f,-0.5f,0.5f),
		glm::vec3(-0.5f,0.5f,0.5f),glm::vec3(0.5f,0.5f,0.5f),

		//top
		glm::vec3(0.5f,0.5f,0.5f),glm::vec3(0.5f,0.5f,-0.5f),
		glm::vec3(-0.5f,0.5f,-0.5f),glm::vec3(0.5f,0.5f,0.5f),
		glm::vec3(-0.5f,0.5f,0.5f),glm::vec3(-0.5f,0.5f,-0.5f),
		//left
		glm::vec3(-0.5f,0.5f,-0.5f),glm::vec3(-0.5f,0.5f,0.5f),
		glm::vec3(-0.5f,-0.5f,0.5f),glm::vec3(-0.5f,0.5f,-0.5f),
		glm::vec3(-0.5f,-0.5f,-0.5f),glm::vec3(-0.5f,-0.5f,0.5f),

		//bottom
		glm::vec3(-0.5f,-0.5f,0.5f),glm::vec3(0.5f,-0.5f,0.5f),
		glm::vec3(0.5f,-0.5f,-0.5f),glm::vec3(-0.5f,-0.5f,0.5f),
		glm::vec3(-0.5f,-0.5f,-0.5f),glm::vec3(0.5f,-0.5f,-0.5f),

		//right
		glm::vec3(0.5f,-0.5f,-0.5f),glm::vec3(0.5f,0.5f,-0.5f),
		glm::vec3(0.5f,0.5f,0.5f),glm::vec3(0.5f,-0.5f,-0.5f),
		glm::vec3(0.5f,-0.5f,0.5f),glm::vec3(0.5f,0.5f,0.5f),
		//back face
		glm::vec3(-0.5f,0.5f,-0.5f),glm::vec3(-0.5f,-0.5f,-0.5f),
		glm::vec3(0.5f,-0.5f,-0.5f),glm::vec3(-0.5f,0.5f,-0.5f),
		glm::vec3(0.5f,0.5f,-0.5f),glm::vec3(0.5f,-0.5f,-0.5f),
	};
	GLuint cubeVAO;
	glGenVertexArrays(1, &cubeVAO);
	glBindVertexArray(cubeVAO);

	GLuint cubeVBO;
	glGenBuffers(1, &cubeVBO);
	glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3)*unitCube.size(), &unitCube[0], GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	//rendering stuff
	while (!glfwWindowShouldClose(window)) {
		//input
		processInput(window);
		//background color
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		//wireframe mode
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);


		//=======================================================
		//WIP!! CAMERA CONTROLS
		//=======================================================

		//model


		// create transformations
		glm::mat4 view;
		glm::mat4 projection;
		//projection = glm::perspective(glm::radians(45.0f), (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT, 0.1f, 100.0f);
		projection = glm::perspective(glm::radians(45.0f + zrad), (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT, 0.1f, 100.0f);
		view = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));
		//Uniform manips
		glUniformMatrix4fv(glGetUniformLocation(shdr, "projection"), 1, GL_FALSE, &projection[0][0]);
		glUniformMatrix4fv(glGetUniformLocation(shdr, "view"), 1, GL_FALSE, &view[0][0]);

		//pos
		glm::mat4 model;
		
		model = glm::rotate(model, glm::radians(45.0f + yrad), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::rotate(model, glm::radians(-45.0f / 2 + xrad), glm::vec3(0.0f, 1.0f, 0.0f));



		model = glm::translate(model, glm::vec3(-50.0f, 0, -50.0f)); //so grid is centered
		glUniformMatrix4fv(glGetUniformLocation(shdr, "model"), 1, GL_FALSE, &model[0][0]);

		//render tiles
		glBindVertexArray(VAO);
		for (int x = -50; x < 49; x++) {
			for (int z = -50; z < 49; z++) {
				model = glm::translate(model, glm::vec3(0.0f, 0.0f, 1 + 0.0f));
				glUniformMatrix4fv(glGetUniformLocation(shdr, "model"), 1, GL_FALSE, &model[0][0]);
				glUniform4f(glGetUniformLocation(shdr, "col"), 1.0f, 1.0f, 1.0f, 1.0f);
				glDrawArrays(GL_QUADS, 0, tile.size());

			}
			model = glm::translate(model, glm::vec3(1+0.0f, 0.0f, 0.0f - 99));
			glUniformMatrix4fv(glGetUniformLocation(shdr, "model"), 1, GL_FALSE, &model[0][0]);

		}
	
		



		//render XYZ axes
		glBindVertexArray(axisVAO);
		model = glm::translate(model, glm::vec3(-49.0f, 0, +50.0f)); //we head back to centre
		glUniformMatrix4fv(glGetUniformLocation(shdr, "model"), 1, GL_FALSE, &model[0][0]);
		glUniform4f(glGetUniformLocation(shdr, "col"), 1.0f, 0.0f, 0.0f, 1.0f);
		glDrawArrays(GL_LINES, 0, 2);

		glUniform4f(glGetUniformLocation(shdr, "col"), 0.0f, 1.0f, 0.0f, 1.0f);
		glDrawArrays(GL_LINES, 2, 2);

		glUniform4f(glGetUniformLocation(shdr, "col"), 0.0f, 0.0f, 1.0f, 1.0f);
		glDrawArrays(GL_LINES, 4, 2);
		

		//cube
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glBindVertexArray(cubeVAO);
		glUniform4f(glGetUniformLocation(shdr, "col"), 0.0f, 1.0f, 1.0f, 1.0f);
		glDrawArrays(GL_TRIANGLES, 0, unitCube.size());


		glfwPollEvents();
		glfwSwapInterval(1);
		glfwSwapBuffers(window);
	}

	return 0;
}
//remaps everything on window resize to fix scaling
void fb_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

void processInput(GLFWwindow *window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
	{
		yrad -= 2.0f; 
		
	}
	if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
	{
		yrad += 2.0f; 	
	}
	if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
	{
		xrad += 2.0f;

	}
	if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
	{
		xrad -= 2.0f;
	}
	if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS)
	{
		zrad -= 2.0f;
	}if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS)
	{
		zrad += 2.0f;
	}
}

GLuint loadShaders(std::string vertex_shader_path, std::string fragment_shader_path) {

	// Create the shaders
	GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
	GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

	// Read the Vertex Shader code from the file
	std::string VertexShaderCode;
	std::ifstream VertexShaderStream(vertex_shader_path, std::ios::in);

	if (VertexShaderStream.is_open()) {
		std::string Line = "";
		while (getline(VertexShaderStream, Line))
			VertexShaderCode += "\n" + Line;
		VertexShaderStream.close();
	}
	else {
		printf("Impossible to open %s. Are you in the right directory ?\n", vertex_shader_path.c_str());
		getchar();
		exit(-1);
	}

	// Read the Fragment Shader code from the file
	std::string FragmentShaderCode;
	std::ifstream FragmentShaderStream(fragment_shader_path, std::ios::in);

	if (FragmentShaderStream.is_open()) {
		std::string Line = "";
		while (getline(FragmentShaderStream, Line))
			FragmentShaderCode += "\n" + Line;
		FragmentShaderStream.close();
	}
	else {
		printf("Impossible to open %s. Are you in the right directory ?\n", fragment_shader_path.c_str());
		getchar();
		exit(-1);
	}

	GLint Result = GL_FALSE;
	int InfoLogLength;

	// Compile Vertex Shader
	printf("Compiling shader : %s\n", vertex_shader_path.c_str());
	char const * VertexSourcePointer = VertexShaderCode.c_str();
	glShaderSource(VertexShaderID, 1, &VertexSourcePointer, nullptr);
	glCompileShader(VertexShaderID);

	// Check Vertex Shader
	glGetShaderiv(VertexShaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(VertexShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if (InfoLogLength > 0) {
		std::vector<char> VertexShaderErrorMessage(InfoLogLength + 1);
		glGetShaderInfoLog(VertexShaderID, InfoLogLength, nullptr, &VertexShaderErrorMessage[0]);
		printf("%s\n", &VertexShaderErrorMessage[0]);
	}

	// Compile Fragment Shader
	printf("Compiling shader : %s\n", fragment_shader_path.c_str());
	char const * FragmentSourcePointer = FragmentShaderCode.c_str();
	glShaderSource(FragmentShaderID, 1, &FragmentSourcePointer, nullptr);
	glCompileShader(FragmentShaderID);

	// Check Fragment Shader
	glGetShaderiv(FragmentShaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(FragmentShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if (InfoLogLength > 0) {
		std::vector<char> FragmentShaderErrorMessage(InfoLogLength + 1);
		glGetShaderInfoLog(FragmentShaderID, InfoLogLength, nullptr, &FragmentShaderErrorMessage[0]);
		printf("%s\n", &FragmentShaderErrorMessage[0]);
	}

	// Link the program
	printf("Linking program\n");
	GLuint ProgramID = glCreateProgram();
	glAttachShader(ProgramID, VertexShaderID);
	glAttachShader(ProgramID, FragmentShaderID);

	glBindAttribLocation(ProgramID, 0, "in_Position");

	glLinkProgram(ProgramID);

	// Check the program
	glGetProgramiv(ProgramID, GL_LINK_STATUS, &Result);
	glGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if (InfoLogLength > 0) {
		std::vector<char> ProgramErrorMessage(InfoLogLength + 1);
		glGetProgramInfoLog(ProgramID, InfoLogLength, nullptr, &ProgramErrorMessage[0]);
		printf("%s\n", &ProgramErrorMessage[0]);
	}

	glDeleteShader(VertexShaderID); //free up memory
	glDeleteShader(FragmentShaderID);

	return ProgramID;
}
