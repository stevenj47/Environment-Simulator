#ifndef _FLOOR_H_
#define _FLOOR_H_

#define GLFW_INCLUDE_GLEXT
#ifdef __APPLE__
#define GLFW_INCLUDE_GLCOREARB
#else
#include <GL/glew.h>
#endif
#include <GLFW/glfw3.h>
// Use of degrees is deprecated. Use radians instead.
#ifndef GLM_FORCE_RADIANS
#define GLM_FORCE_RADIANS
#endif
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>


class Floor 
{
public:
	Floor();
	~Floor();

	void draw(GLuint shader);
	//void update();
	//void spin(float);

	// These variables are needed for the shader program
	GLuint VBO, VAO, EBO, textureID;
	GLuint uProjection, uModelview;

	glm::mat4 toWorld = glm::mat4(1.0f);

};

#endif

