#ifndef _WINDOW_H_
#define _WINDOW_H_

#include <iostream>

#define GLFW_INCLUDE_GLEXT
#define _USE_MATH_DEFINES

#ifdef __APPLE__
#define GLFW_INCLUDE_GLCOREARB
#else
#include <GL/glew.h>
#endif
#include <GLFW/glfw3.h>
#include <time.h>
#include <math.h>
#include <algorithm>
#include "Cube.h"
#include "Sphere.h"
#include "Skybox.h"
#include "shader.h"
#include "Floor.h"
#include "Terrain.h"
#include "SOIL.h"

class Window
{
public:
	static int width;
	static int height;
	static glm::mat4 P; // P for projection
	static glm::mat4 V; // V for view
	static glm::mat4 lightP; // P for projection
	static glm::mat4 cascadeLightPs[3];
	static glm::mat4 lightV; // V for view
	static bool cullStatus;
	static GLfloat planes[6][4]; // For frustrum
	static void initialize_objects();
	static void clean_up();
	static GLFWwindow* create_window(int width, int height);
	static void resize_callback(GLFWwindow* window, int width, int height);
	static void idle_callback();
	static void display_callback(GLFWwindow*);
	static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
	static void mouse_callback(GLFWwindow* window, int button, int action, int mods);
	static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
	static void cursor_position_callback(GLFWwindow* window, double xpos, double ypos);
	static void genDepthMap(unsigned int s_width, unsigned int s_height);
	static void writeDepthMap(unsigned int s_width, unsigned int s_height, int idx);
	static void genWaterBuffers();
	static void writeWaterBuffers();
	static void init_textures();
};

#endif
