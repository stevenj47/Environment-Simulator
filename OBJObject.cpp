#include <iostream>
#include "OBJObject.h"
#include "Window.h"

OBJObject::OBJObject(const char *filepath) 
{
	toWorld = glm::mat4(1.0f);
	name = filepath;

	parse(filepath);

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);
	glGenBuffers(1, &NBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLfloat), &vertices[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0,// This first parameter x should be the same as the number passed into the line "layout (location = x)" in the vertex shader. In this case, it's 0. Valid values are 0 to GL_MAX_UNIFORM_LOCATIONS.
		3, // This second line tells us how any components there are per vertex. In this case, it's 3 (we have an x, y, and z component)
		GL_FLOAT, // What type these components are
		GL_FALSE, // GL_TRUE means the values should be normalized. GL_FALSE means they shouldn't
		3 * sizeof(GLfloat), // Offset between consecutive indices. Since each of our vertices have 3 floats, they should have the size of 3 floats in between
		(GLvoid*)0); // Offset of the first vertex's component. In our case it's 0 since we don't pad the vertices array with anything.

	glBindBuffer(GL_ARRAY_BUFFER, NBO);
	glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(GLfloat), &normals[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1,
		3,
		GL_FLOAT,
		GL_FALSE,
		3 * sizeof(GLfloat),
		(GLvoid*)0);
	// We've sent the vertex data over to OpenGL, but there's still something missing.
	// In what order should it draw those vertices? That's why we'll need a GL_ELEMENT_ARRAY_BUFFER for this.
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), indices.data(), GL_STATIC_DRAW);

	// Unbind the currently bound buffer so that we don't accidentally make unwanted changes to it.
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	// Unbind the VAO now so we don't accidentally tamper with it.
	// NOTE: You must NEVER unbind the element array buffer associated with a VAO!
	glBindVertexArray(0); 
}

OBJObject::~OBJObject()
{
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);
	glDeleteBuffers(1, &NBO);
}

void OBJObject::parse(const char *filepath) 
{
	// Populate the face indices, vertices, and normals vectors with the OBJ Object data
	FILE* fp;
	float x, y, z;
	float tx, ty;

	int c1, c2;
	unsigned int fv1;
	unsigned int nv1, tv1, fv2, tv2, nv2, fv3, tv3, nv3;

	std::vector<GLuint> vertex_data;
	std::vector<GLfloat> temp_vertices;
	std::vector<GLfloat> temp_normals;
	std::vector<GLfloat> temp_tex_coords;

	fp = fopen(filepath, "rb");
	
	if (fp == NULL) {
		std::cerr << "File not found." << std::endl;
	}
        
	c1 = fgetc(fp);
	c2 = fgetc(fp); 
	int i = 0;

	while(c1 != EOF) {

		if(c1 == 'v' && c2 == ' ') {

			fscanf(fp, "%f %f %f\n", &x, &y, &z);
			temp_vertices.push_back(x);
			temp_vertices.push_back(y);
			temp_vertices.push_back(z);

	//		fprintf(stderr, "%f %f %f\n", x, y, z);
		} else if (c1 == 'v' && c2 == 'n') {

			fscanf(fp, "%f %f %f\n", &x, &y, &z);
			temp_normals.push_back(x);
			temp_normals.push_back(y);
			temp_normals.push_back(z);

		} else if (c1 == 'v' && c2 == 't') {

			fscanf(fp, "%f %f\n", &tx, &ty);
			temp_tex_coords.push_back(tx);
			temp_tex_coords.push_back(ty);

		} else if (c1 == 'f' || c2 == ' ') {
			vertex_data.resize(vertex_data.size() + 9);
			fscanf(fp, "%u/%u/%u %u/%u/%u %u/%u/%u\n", &vertex_data[0], &vertex_data[1], &vertex_data[i++], &vertex_data[i++], &vertex_data[i++], &vertex_data[i++], &vertex_data[i++], 
				&vertex_data[i++], &vertex_data[i++]);		
		} else {
			fprintf(stderr, "%c%c", c1, c2);
		}

		c1 = fgetc(fp);
		c2 = fgetc(fp);
	}

	for (int i = 0; i < vertex_data.size(); i++)
	{
		int v_data = vertex_data.at(i);
		int t_data = vertex_data.at(i + 1);
		int n_data = vertex_data.at(i + 2);
		fprintf(stderr, "%i %i %i %i\n", i, (int)v_data, (int)t_data, (int)n_data);

		vertices.push_back(temp_vertices.at(3 * (v_data - 1)));
		normals.push_back(temp_normals.at(3 * (n_data - 1)));
		tex_coords.push_back(temp_tex_coords.at(2 * (t_data - 1)));
	} 

	fprintf(stderr, "V: %i, N: %i, F: %i, T: %i\n", vertices.size(), normals.size(), indices.size(), tex_coords.size());
	fclose(fp); 
}

void OBJObject::render(GLuint shaderProgram) 
{
	uProjection = glGetUniformLocation(shaderProgram, "projection");
	uModel = glGetUniformLocation(shaderProgram, "model");
	uView = glGetUniformLocation(shaderProgram, "view");

	// Now send these values to the shader program
	glUniformMatrix4fv(uProjection, 1, GL_FALSE, &Window::P[0][0]);
	glUniformMatrix4fv(uModel, 1, GL_FALSE, &toWorld[0][0]);
	glUniformMatrix4fv(uView, 1, GL_FALSE, &Window::V[0][0]);

	glBindVertexArray(VAO);
	// Tell OpenGL to draw with triangles, using 36 indices, the type of the indices, and the offset to start from
	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
	// Unbind the VAO when we're done so we don't accidentally draw extra stuff or tamper with its bound buffers
	glBindVertexArray(0);
}

