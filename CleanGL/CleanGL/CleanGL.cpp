// CleanGL.cpp : Defines the entry point for the console application.
//
/*
	EARL STEVEN AROMIN 40004997
*/
/*

Vector to VBO
VAO tells what VBO does

*/
#include "stdafx.h"
#include <vector>
#include <glew/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <fstream>
#include <string>
#include <glm/gtc/type_ptr.hpp>
#include <stdlib.h>    
#include <time.h> 
///USER INCLUDES
#include "Cube.h"
#include "Map.h"
#include "Horse.h"
//SHADER PROGRAM
GLuint shdr;

void fb_size_callback(GLFWwindow* window, int w, int h);
GLuint loadShaders(std::string vertex_shader_path, std::string fragment_shader_path);
void processInput(GLFWwindow *window);
void update_view();

const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 800;
GLFWwindow* window;

float xrad = 0.0f;
float yrad = 0.0f;
float zrad = 0.0f; //currently used for zooming. can it be used l;ater?
 //view matrix // camera
glm::vec3 c_pos = glm::vec3(0, 0, 2);
glm::vec3 c_dir = glm::normalize(glm::vec3(0, 0, -2));
glm::vec3 c_up = glm::vec3(0, 1, 0);
//scale
glm::mat4 scale;
//vm
glm::mat4 vm;

glm::mat4 projection;

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
int init() {

	srand(time(NULL));
	glfwInit();

	window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Assignment 1", nullptr, nullptr);
	if (window == nullptr) {
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);
	if (glewInit() != 0) {
		return -1;
	}

	glfwSetFramebufferSizeCallback(window, fb_size_callback);

	return 0;
}

int main()
{

	if (init() != 0) {
		return -1;
	}
	 shdr = loadShaders("v.s", "f.s");
	 //so stuff draws properly
	 Map::set_shader(shdr);
	glUseProgram(shdr);


	GLuint cubeVAO;
	glGenVertexArrays(1, &cubeVAO);
	glBindVertexArray(cubeVAO);

	GLuint cubeVBO;
	glGenBuffers(1, &cubeVBO);
	glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3)*unitCube.size(), &unitCube[0], GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);



	
	projection = glm::perspective(glm::radians(45.0f), (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT, 0.1f, 200.0f);
	Horse smol_horse;
	
	update_view();
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
		glm::mat4 model;
		model = glm::rotate(model, glm::radians(45.0f + yrad), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::rotate(model, glm::radians(-45.0f / 2 + xrad), glm::vec3(0.0f, 1.0f, 0.0f));
		//make sure everything is properly scaled
		scale = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 1.0f, 1.0f));
		glUniformMatrix4fv(glGetUniformLocation(shdr, "scale"), 1, GL_FALSE, glm::value_ptr(scale));
		vm = glm::translate(vm, glm::vec3(0.0f, 0.0f, -3.0f));
		//Uniform manips
		glUniformMatrix4fv(glGetUniformLocation(shdr, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(glGetUniformLocation(shdr, "view"), 1, GL_FALSE, glm::value_ptr(vm));
		
		
		//MAP
		Map::set_model(model);
		Map::draw();
		update_view();
		
		glEnable(GL_DEPTH_TEST);//SO WE GET PROPER LAYERS

		//======================================================
		//HORSE MODE YES
		//======================================================
		//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		smol_horse.set_model(model);
		smol_horse.set_shader(shdr);
		smol_horse.horse_controller(window);
		smol_horse.draw();
		
		glDisable(GL_DEPTH_TEST);

		///JUST TO LOCATE WHERE THE FINAL MODEL COORD IS
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		scale = glm::scale(glm::mat4(1.0f), glm::vec3(0.05f, 0.05f, 0.05f));
		glUniformMatrix4fv(glGetUniformLocation(shdr, "scale"), 1, GL_FALSE, glm::value_ptr(scale));
		glUniformMatrix4fv(glGetUniformLocation(shdr, "model"), 1, GL_FALSE, glm::value_ptr(model));
		glUniform4f(glGetUniformLocation(shdr, "col"), 1.0f, 0.0f, 0.0f, 1.0f);
		//glDrawArrays(GL_TRIANGLES, 0, unitCube.size());
		Cube::set();
		Cube::draw();

	

		glfwPollEvents();
		glfwSwapInterval(1);
		glfwSwapBuffers(window);
	}

	return 0;
}

void update_view() {

	vm = glm::lookAt(c_pos, c_pos + c_dir, c_up);
}
//remaps everything on window resize to fix scaling
//w and h are just there to satisfy the callback args
void fb_size_callback(GLFWwindow* window, int w, int h)
{
	int width, height;
	glfwGetWindowSize(window, &width, &height);
	projection = glm::perspective(glm::radians(45.0f),(float) width / height, 0.1f, 200.0f);

	glViewport(0, 0, width, height);
}


void processInput(GLFWwindow *window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
	{
		yrad += 2.0f;
		update_view();
	}
	if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
	{
		yrad -= 2.0f;
		update_view();
	}
	if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
	{
		xrad -= 2.0f;
		update_view();

	}
	if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
	{
		xrad += 2.0f;
		update_view();
	}
	if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS)
	{
		if (c_pos.z > 2)
			c_pos.z -= 1.0f;
		update_view();
	}if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS)
	{
		c_pos.z += 1.0f;
		update_view();
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
