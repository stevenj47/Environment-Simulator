#include "Floor.h"
#include "Window.h"
#include "SOIL.h"

//FLAT FLOOR
GLfloat floorVertices[] = {
	// Positions          // Normals         // Texture Coords
	50.0f, 0.0f, 50.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
	-50.0f, 0.0f, -50.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
	-50.0f, 0.0f, 50.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f,

	50.0f, 0.0f, 50.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
	50.0f, 0.0f, -50.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f,
	-50.0f, 0.0f, -50.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f
};


Floor::Floor()
{
	// Set up plane/floor
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(floorVertices), &floorVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

Floor::~Floor()
{
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
}

void Floor::draw(GLuint shaderProgram) {

	glUseProgram(shaderProgram);

	glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"), 1, GL_FALSE, &Window::P[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, &toWorld[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "view"), 1, GL_FALSE, &Window::V[0][0]);

	glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "light_proj"), 1, GL_FALSE, &Window::lightP[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "light_proj0"), 1, GL_FALSE, &(Window::cascadeLightPs[0])[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "light_proj1"), 1, GL_FALSE, &(Window::cascadeLightPs[1])[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "light_proj2"), 1, GL_FALSE, &(Window::cascadeLightPs[2])[0][0]);

	glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "light_view"), 1, GL_FALSE, &Window::lightV[0][0]);

	glBindVertexArray(VAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);

}



