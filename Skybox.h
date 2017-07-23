#ifndef _SKYBOX_H_
#define _SKYBOX_H_

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

class SkyBox
{
public:
	SkyBox();
	~SkyBox();

	glm::mat4 toWorld;

	void draw(GLuint);

	// These variables are needed for the shader program
	GLuint VBO, VAO, EBO, textureID;
	GLuint uProjection, uModelview;
	std::vector<const GLchar*> faces;
};

// Define the coordinates and indices needed to draw the cube. Note that it is not necessary
// to use a 2-dimensional array, since the layout in memory is the same as a 1-dimensional array.
// This just looks nicer since it's easy to tell what coordinates/indices belong where.
const GLfloat sky_vertices[8][3] = {
	// "Front" vertices
	{-20.0, -20.0,  20.0}, {20.0, -20.0,  20.0}, {20.0,  20.0,  20.0}, {-20.0,  20.0,  20.0},
	// "Back" vertices
	{-20.0, -20.0, -20.0}, {20.0, -20.0, -20.0}, {20.0,  20.0, -20.0}, {-20.0,  20.0, -20.0}
};

// Note that GL_QUADS is deprecated in modern OpenGL (and removed from OSX systems).
// This is why we need to draw each face as 2 triangles instead of 1 quadrilateral
const GLuint sky_indices[6][6] = {
	// Front face
	{0, 2, 1, 2, 0, 3},
	// Top face
	{1, 6, 5, 6, 1, 2},
	// Back face
	{7, 5, 6, 5, 7, 4},
	// Bottom face
	{4, 3, 0, 3, 4, 7},
	// Left face
	{4, 1, 5, 1, 4, 0},
	// Right face
	{3, 6, 2, 6, 3, 7}
};

#endif


