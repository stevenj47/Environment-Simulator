#ifndef _TERRAIN_H_
#define _TERRAIN_H_

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

class Terrain
{
public:
	Terrain(int x, int y);
	~Terrain();

	glm::mat4 toWorld = glm::mat4(1.0f);
	std::vector<GLfloat> vertices;
	std::vector<GLfloat> normals;
	std::vector<GLfloat> tex_coords;
	std::vector<GLuint> indices;

	bool render(GLuint);
	void calcNormals(int v1_idx, int v2_idx, int v3_idx);

	GLuint VBO, NBO, TBO, VAO, EBO, textureID;
	GLuint uProjection, uView, uModel;

	const int size = 800;

	int gridX, gridY;
};

#endif

