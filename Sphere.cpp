#include "Sphere.h"
#include "Window.h"

int Sphere::stacks = 32;
int Sphere::slices = 32;
float Sphere::radius = 5.0f;

Sphere::Sphere(GLuint shader)
{
	genVertices(stacks, slices);

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);
	glGenBuffers(1, &NBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLfloat), &vertices[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0); 

	glBindBuffer(GL_ARRAY_BUFFER, NBO);
	glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(GLfloat), &normals[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1,
		3,
		GL_FLOAT,
		GL_FALSE,
		3 * sizeof(GLfloat),
		(GLvoid*)0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), indices.data(), GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(0);
}

Sphere::~Sphere()
{
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);
	glDeleteBuffers(1, &NBO);
}

bool Sphere::render(GLuint shader)
{
	glUseProgram(shader);

	uProjection = glGetUniformLocation(shader, "projection");
	GLuint uModel = glGetUniformLocation(shader, "model");
	GLuint uView = glGetUniformLocation(shader, "view");

	glUniformMatrix4fv(uProjection, 1, GL_FALSE, &Window::P[0][0]);
	glUniformMatrix4fv(uModel, 1, GL_FALSE, &toWorld[0][0]);
	glUniformMatrix4fv(uView, 1, GL_FALSE, &Window::V[0][0]);

	glUniformMatrix4fv(glGetUniformLocation(shader, "light_proj"), 1, GL_FALSE, &Window::lightP[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(shader, "light_proj0"), 1, GL_FALSE, &(Window::cascadeLightPs[0])[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(shader, "light_proj1"), 1, GL_FALSE, &(Window::cascadeLightPs[1])[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(shader, "light_proj2"), 1, GL_FALSE, &(Window::cascadeLightPs[2])[0][0]);

	glUniformMatrix4fv(glGetUniformLocation(shader, "light_view"), 1, GL_FALSE, &Window::lightV[0][0]);

	glBindVertexArray(VAO);

	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);

	glBindVertexArray(0);

	return false;
}


void Sphere::genVertices(int stacks, int slices)
{
	// From http://learningwebgl.com/blog/?p=1253
	// Calculate the vertices
	for (int i = 0; i <= stacks; ++i)
	{
		float phi = i * glm::pi <float>() / float(stacks);

		for (int j = 0; j <= slices; ++j)
		{
			float theta = 2 * j * glm::pi <float>() / float(slices);

			float x = cosf(theta) * sinf(phi);
			float y = cosf(phi);
			float z = sinf(theta) * sinf(phi);

			normals.push_back(x);
			normals.push_back(y);
			normals.push_back(z);

			x *= radius;
			y *= radius;
			z *= radius;

			vertices.push_back(x);
			vertices.push_back(y);
			vertices.push_back(z);
		}
	}

	// Calculate the faces
	for (int i = 0; i < slices * stacks + slices; ++i)
	{
		indices.push_back(i);
		indices.push_back(i + slices + 1);
		indices.push_back(i + slices);

		indices.push_back(i + slices + 1);
		indices.push_back(i);
		indices.push_back(i + 1);
	}
}

