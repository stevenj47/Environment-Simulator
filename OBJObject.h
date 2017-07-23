#ifndef OBJOBJECT_H
#define OBJOBJECT_H

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

class OBJObject
{
public:
	OBJObject(const char* filepath);
	~OBJObject();
	glm::mat4 toWorld;
	const char * name;

	void parse(const char* filepath);
	void render(GLuint);

	std::vector<GLfloat> vertices;
	std::vector<GLfloat> normals;
	std::vector<GLfloat> tex_coords;
	std::vector<GLuint> indices;

	GLuint VBO, VAO, EBO, NBO, TBO;
	GLuint uProjection, uModel, uView;

};

#endif
