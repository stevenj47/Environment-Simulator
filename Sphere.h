#ifndef _SPHERE_H_
#define _SPHERE_H_

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

class Sphere
{
public:
	Sphere(GLuint shader);
	~Sphere();

	bool render(GLuint shader);
	void genVertices(int stacks, int slices);

	static int stacks;
	static int slices;
	static float radius;

	GLuint VBO, VAO, EBO, NBO;
	GLuint uProjection, uModelview;

	glm::mat4 toWorld = glm::mat4(1.0f);

	std::vector<GLfloat> vertices;
	std::vector<GLfloat> normals;
	std::vector<GLuint> indices;
};
#endif