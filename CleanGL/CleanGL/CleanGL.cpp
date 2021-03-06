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
#include "Horse_Animator.h"
#include "Bounding_Sphere.h"

//SHADER PROGRAM
GLuint shdr,depthshdr,debugshdr;
GLuint a, b;
int width = 800; 
int height = 800;
GLuint* depthFBO;
GLuint* depthMap;
void frame_size_callback(GLFWwindow* window, int w, int h);
GLuint loadShaders(std::string vertex_shader_path, std::string fragment_shader_path);
void configureDepthFBO();
void processInput(GLFWwindow *window);
void update_view();
void draw_world(GLuint *shader);
const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 800;
const unsigned int SHADOW_WIDTH = 2048, SHADOW_HEIGHT = 2048;
GLFWwindow* window;
void renderQuad(); // FROM https://learnopengl.com/Advanced-Lighting/Shadows/Shadow-Mapping
float xrad = 0.0f;
float yrad = 0.0f;
glm::vec3 camera_movlog(0.0f, 0.0f, 0.0f);
//view matrix // camera
glm::vec3 c_pos = glm::vec3(0, 0, 2);
glm::vec3 c_dir = glm::normalize(glm::vec3(0, 0, -2));
glm::vec3 c_up = glm::vec3(0, 1, 0);
glm::vec3 light_position = glm::vec3(0, 110, 0.01);
//scale
glm::mat4 scale;
//vm
glm::mat4 vm;

glm::mat4 projection;
glm::mat4 model;
//rendering modes
bool mode_line = false;
bool mode_trig = true;
bool mode_point = false;
bool enable_textures = false;
bool enable_shadows = false;
bool enable_animations = false;
double mouse_x, mouse_y;
Horse *troupe[20];
Bounding_Sphere *touchy_bols[20];
Bounding_Sphere *god_bol;
Horse_Animator *moving_horsies[20];

Horse smol_horse;
Horse_Animator testani;
int init() {
	depthFBO = &a;
	depthMap = &b;
	srand(time(NULL));

	glfwInit();

	window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Final Project", nullptr, nullptr);
	if (window == nullptr) {
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);
	if (glewInit() != 0) {
		return -1;
	}

	glfwSetFramebufferSizeCallback(window, frame_size_callback);

	return 0;

	smol_horse.is_god = true;
	
}

int main()
{

	if (init() != 0) {
		return -1;
	}

	shdr = loadShaders("v.glsl", "f.glsl");
	depthshdr = loadShaders("shadow_map_vs.glsl", "shadow_map_fs.glsl");
	debugshdr =  loadShaders("shadow_quad_vs.glsl", "shadow_quad_fs.glsl");
	configureDepthFBO();

	glUseProgram(shdr);
	glUniform1i(glGetUniformLocation(shdr, "shadowMap"), 1);
	glUseProgram(debugshdr);
	glUniform1i(glGetUniformLocation(debugshdr, "shadowMap"), 1);
	//so stuff draws properly
	Map::set_shader(shdr);
	glm::mat4 id = glm::mat4(1.0f);
	Map::set_model(id);
	Cube::set_shader(shdr);
	Cube::init();
	Floor::set(shdr);
	Floor::init();
	//light
	projection = glm::perspective(glm::radians(45.0f), (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT, 0.1f, 200.0f);
	god_bol = new Bounding_Sphere(smol_horse);

	for (int i = 0; i < 20; i++) {
		int randx = rand() % 90 - 45;
		int randz = rand() % 90 - 45;
		troupe[i] = new Horse((float)randx, 0, (float)randz);
		
		moving_horsies[i] = new Horse_Animator(*troupe[i], (rand()%360));
		touchy_bols[i] = new Bounding_Sphere(*troupe[i]);
	}
	update_view();
	//rendering stuff
	while (!glfwWindowShouldClose(window)) {
		

		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// 1. render depth of scene to texture (from light's perspective)
		// --------------------------------------------------------------
		glm::mat4 lightProjection, lightView;
		glm::mat4 lightSpaceMatrix;
		float near_plane = 20.1f, far_plane = 200.f;
		lightProjection = glm::perspective(glm::radians(45.0f), (GLfloat)SHADOW_WIDTH / (GLfloat)SHADOW_HEIGHT, near_plane, far_plane); 
		lightView = glm::lookAt(light_position  , glm::vec3(0.01f,0.0f,0.0f) , glm::vec3(0.0, 1.0, 0.0));
		lightSpaceMatrix = lightProjection * lightView;
		// render scene from light's point of view
		glUseProgram(depthshdr);
		glUniformMatrix4fv(glGetUniformLocation(depthshdr, "lightSpaceMatrix"), 1, GL_FALSE, glm::value_ptr(lightSpaceMatrix));


		glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
		glBindFramebuffer(GL_FRAMEBUFFER, *depthFBO);
		glClear(GL_DEPTH_BUFFER_BIT);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, Cube::texture);
		draw_world(&depthshdr);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	//========================================================
		
	  glViewport(0,0, width, height);
	  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glUseProgram(shdr);
		if (enable_shadows)
			glUniform1i(glGetUniformLocation(shdr, "shadowtoggle"),1);
		else
			glUniform1i(glGetUniformLocation(shdr, "shadowtoggle"), 0);
		glUniform3fv(glGetUniformLocation(shdr, "light_position"), 1, glm::value_ptr(light_position));
		glUniformMatrix4fv(glGetUniformLocation(shdr, "light_space"), 1, GL_FALSE, glm::value_ptr(lightSpaceMatrix));

		//input
		processInput(window);
		//background color
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		//wireframe mode
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

		glUniform3fv(glGetUniformLocation(shdr, "view_position"), 1, glm::value_ptr(c_pos));
		

	
		//make sure everything is properly scaled
		scale = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 1.0f, 1.0f));
		glUniformMatrix4fv(glGetUniformLocation(shdr, "scale"), 1, GL_FALSE, glm::value_ptr(scale));
		vm = glm::translate(vm, glm::vec3(0.0f, 0.0f, -3.0f));


		vm = glm::rotate(vm, glm::radians(45.0f + yrad), glm::vec3(1.0f, 0.0f, 0.0f));
		vm = glm::rotate(vm, glm::radians(-45.0f / 2 + xrad), glm::vec3(0.0f, 1.0f, 0.0f));


		glUniformMatrix4fv(glGetUniformLocation(shdr, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(glGetUniformLocation(shdr, "view"), 1, GL_FALSE, glm::value_ptr(vm));
		glUniformMatrix4fv(glGetUniformLocation(shdr, "lightSpaceMatrix"), 1, GL_FALSE, glm::value_ptr(lightSpaceMatrix));

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, Cube::texture);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, *depthMap);
		draw_world(&shdr);
		
//=====================================================

	
		glUseProgram(debugshdr);
		glUniform1f(glGetUniformLocation(debugshdr, "near_plane"), near_plane);
		glUniform1f(glGetUniformLocation(debugshdr, "far_plane"), far_plane);

		
		glActiveTexture(GL_TEXTURE0);

		glBindTexture(GL_TEXTURE_2D, *depthMap);
		if (glfwGetKey(window, GLFW_KEY_M) == GLFW_PRESS) {
			renderQuad();
		}
		//Cube::release();
		Bounding_Sphere::update_collision();

		glfwPollEvents();
		glfwSwapInterval(1);
		glfwSwapBuffers(window);
	}

	return 0;
}
void draw_world(GLuint *shader) {
	
	Map::set_shader(*shader);
	glm::mat4 id = glm::mat4(1.0f);
	Map::set_model(id);
	Cube::set_shader(*shader);

	Floor::set(*shader);
	//MAP		

	Map::draw();
	update_view();

	glEnable(GL_DEPTH_TEST);//SO WE GET PROPER LAYERS


	if (mode_trig) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}
	else if (mode_line) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	}
	else if (mode_point) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);

	}
	glUniform1f(glGetUniformLocation(*shader, "ambientIntensity"), .25f);

	Floor::draw();


	Horse::set_shader(*shader);

	
	smol_horse.horse_controller(window);
	smol_horse.set_model(model);

	smol_horse.draw();
	for (int i = 0; i < 20; i++) {
		troupe[i]->set_model(model);
		
		troupe[i]->draw();
	
		if(enable_animations && !(troupe[i]->stopped)){
			
			moving_horsies[i]->animation_loop();
			
		}
	}



	glDisable(GL_DEPTH_TEST);
}
void update_view() {

	vm = glm::lookAt(c_pos, c_pos + c_dir, c_up);
}
//remaps everything on window resize to fix scaling
//w and h are just there to satisfy the callback args

void frame_size_callback(GLFWwindow* window, int w, int h)
{

	glfwGetWindowSize(window, &width, &height);
	projection = glm::perspective(glm::radians(45.0f), (float)width / height, 0.1f, 200.0f);

	glViewport(0, 0, width, height);
}


bool home_press = false;
bool x_press = false;
bool b_press = false;
bool h_press = false;
void processInput(GLFWwindow *window)
{

	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
	if (glfwGetKey(window, GLFW_KEY_HOME) == GLFW_PRESS)
	{
		xrad = 0.0f;
		yrad = 0.0f;
		c_pos -= camera_movlog;
		camera_movlog = glm::vec3(0.0f, 0.0f, 0.0f);
		smol_horse.rotation = glm::mat4(1.0f);
		smol_horse.movement_log = glm::vec3(0.0f, 0.0f, 0.0f);
		update_view();

	}

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
		xrad -= 5.0f;
		update_view();

	}
	if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
	{
		xrad += 5.0f;
		update_view();
	}

	//RENDER MDOES
	if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS) {
		mode_line = false;
		mode_trig = false;
		mode_point = true;
	}
	if (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS) {
		mode_line = false;
		mode_trig = true;
		mode_point = false;
	}
	if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS) {
		mode_line = true;
		mode_trig = false;
		mode_point = false;
	}

	//TEXTURES
	if ((glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS))
	{
		x_press = true;
	}
	else if ((glfwGetKey(window, GLFW_KEY_X) == GLFW_RELEASE))
	{
		if (x_press) {

			if (enable_textures){
				Cube::tex_toggle = true;
				Floor::tex_toggle = true;
				enable_textures = false;

			}
			else {
				Cube::tex_toggle = false;
				Floor::tex_toggle = false;
				enable_textures = true;

			}

		}
		x_press = false;
	}
	//SHADOWS
	if ((glfwGetKey(window, GLFW_KEY_B) == GLFW_PRESS))
	{

		b_press = true;
	}
	else if ((glfwGetKey(window, GLFW_KEY_B) == GLFW_RELEASE))
	{
		if (b_press) {

			if (enable_shadows) {
				enable_shadows = false;
			}
			else {
				enable_shadows = true;

			}

		}
		b_press = false;
	}
	//MOUSE CONTROL
	//Panning
	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS) {
		double temp_x, temp_y;
		glfwGetCursorPos(window, &temp_x, &temp_y);
		double delta_x = (temp_x - mouse_x);


		if (delta_x < 0) {

			c_pos.x -= 0.2f;
			camera_movlog.x -= 0.2f;
		}
		else if (delta_x > 0) {

			c_pos.x += 0.2f;
			camera_movlog.x += 0.2f;
		}

		mouse_x = temp_x;
		update_view();

	}else
	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
		//zooming
		double temp_x, temp_y;
		glfwGetCursorPos(window, &temp_x, &temp_y);
		double delta_y = (temp_y - mouse_y);
		if (delta_y < 0) {
			if (c_pos.z > 2) {
				c_pos.z -= 1.0f;
				camera_movlog.z -= 1.0f;
			}
		}
		
		else if (delta_y > 0) {
			c_pos.z += 1.0f;
			camera_movlog.z += 1.0f;	
		}

		mouse_y = temp_y;
		update_view();

	}
	//TILTING

	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_MIDDLE) == GLFW_PRESS) {
		//zooming
		double temp_x, temp_y;
		std::cout << testani.flip_coin() << std::endl;
		glfwGetCursorPos(window, &temp_x, &temp_y);
		double delta_y = (temp_y - mouse_y);
		if (delta_y < 0) {
			yrad -= 2.0f;
			update_view();
		}

		else if (delta_y > 0) {
			yrad += 2.0f;
			update_view();
		}

		mouse_y = temp_y;
		update_view();

	}


	//ani
	if ((glfwGetKey(window, GLFW_KEY_H) == GLFW_PRESS))
	{
		h_press = true;
	}
	else if ((glfwGetKey(window, GLFW_KEY_H) == GLFW_RELEASE))
	{
		if (h_press) {

			if (enable_animations) {
				enable_animations = false;
			}
			else {
				enable_animations = true;

			}

		}
		h_press = false;
	}
}

void configureDepthFBO() {
	glGenFramebuffers(1, &(*depthFBO));
	glGenTextures(1, &(*depthMap));
	glBindTexture(GL_TEXTURE_2D, *depthMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	float borderColor[] = { 1.0, 1.0, 1.0, 1.0 };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
	// attach depth texture as FBO's depth buffer
	glBindFramebuffer(GL_FRAMEBUFFER, *depthFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, *depthMap, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
};

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


unsigned int quadVAO = 0;
unsigned int quadVBO;
void renderQuad()
{
	if (quadVAO == 0)
	{
		float quadVertices[] = {
			// positions        // texture Coords
			-1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
			-1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
			1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
			1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
		};
		// setup plane VAO
		glGenVertexArrays(1, &quadVAO);
		glGenBuffers(1, &quadVBO);
		glBindVertexArray(quadVAO);
		glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	}
	glBindVertexArray(quadVAO);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glBindVertexArray(0);
}