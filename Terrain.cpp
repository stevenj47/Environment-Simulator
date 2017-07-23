#include "Terrain.h"
#include "Window.h"
#include "SOIL.h"

Terrain::Terrain(int x, int y)
{
	gridX = x;
	gridY = y;

	// Load heightmap
	int width, height;
	unsigned char * image_data = SOIL_load_image("textures/HeightMap2.jpg", &width, &height, 0, SOIL_LOAD_L);
	
	int vertex_num = height;

	int v_count = vertex_num * vertex_num;
	vertices.resize(v_count * 3);
	normals.resize(v_count * 3);
	
	tex_coords.resize(v_count * 2);
	indices.resize(6 * (vertex_num - 1) * (vertex_num - 1));
	int curr_vertex = 0;
	float min_h = 1000.0;

	// Generate vertices and normals
	for (int x = 0; x < vertex_num; x++)
	{
		for (int y = 0; y < vertex_num; y++)
		{
			vertices[curr_vertex * 3] = (float)y / ((float)vertex_num - 1) * size;
			vertices[curr_vertex * 3 + 1] = (float)image_data[y * vertex_num + x] - 120.0;
			vertices[curr_vertex * 3 + 2] = (float)x / ((float)vertex_num - 1) * size;

			float heightL = (float)image_data[y * vertex_num + x - 1];
			float heightR = (float)image_data[y * vertex_num + x + 1];
			float heightD = (float)image_data[(y - 1) * vertex_num + x];
			float heightU = (float)image_data[(y + 1) * vertex_num + x];

			normals[curr_vertex * 3] = 0.0;
			normals[curr_vertex * 3 + 1] = 0.0;
			normals[curr_vertex * 3 + 2] = 0.0; 

			tex_coords[curr_vertex * 2] = (float)y / ((float)vertex_num - 1);
			tex_coords[curr_vertex * 2 + 1] = (float)x / ((float)vertex_num - 1);
			curr_vertex++;
		}
	}

	int vertex_loc = 0;

	// Generate triangles
	for (int x = 0; x < vertex_num - 1; x++)
	{
		for (int y = 0; y < vertex_num - 1; y++)
		{
			int loc_tl = (x * vertex_num) + y;
			int loc_tr = loc_tl + 1;
			int loc_bl = ((x + 1) * vertex_num) + y;
			int loc_br = loc_bl + 1;

			indices[vertex_loc++] = loc_tl;
			indices[vertex_loc++] = loc_bl;
			indices[vertex_loc++] = loc_tr;

			calcNormals(3 * loc_tl, 3 * loc_bl, 3 * loc_tr);

			indices[vertex_loc++] = loc_tr;
			indices[vertex_loc++] = loc_bl;
			indices[vertex_loc++] = loc_br;

			// Calculate normals
			calcNormals(3 * loc_tr, 3 * loc_bl, 3 * loc_br);
		}
	}

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &NBO);
	glGenBuffers(1, &TBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLfloat), &vertices[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);

	glBindBuffer(GL_ARRAY_BUFFER, NBO);
	glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(GLfloat), &normals[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_TRUE, 3 * sizeof(GLfloat), (GLvoid*)0);

	glBindBuffer(GL_ARRAY_BUFFER, TBO);
	glBufferData(GL_ARRAY_BUFFER, tex_coords.size() * sizeof(GLfloat), &tex_coords[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (GLvoid*)0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), &indices[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

Terrain::~Terrain()
{
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &NBO);
	glDeleteBuffers(1, &TBO);
	glDeleteBuffers(1, &EBO);
}

bool Terrain::render(GLuint shader)
{
	glUseProgram(shader);

	uProjection = glGetUniformLocation(shader, "projection");
	GLuint uModel = glGetUniformLocation(shader, "model");
	GLuint uView = glGetUniformLocation(shader, "view");

	glUniformMatrix4fv(uProjection, 1, GL_FALSE, &Window::P[0][0]);
	glUniformMatrix4fv(uModel, 1, GL_FALSE, &toWorld[0][0]);
	glUniformMatrix4fv(uView, 1, GL_FALSE, &Window::V[0][0]);

	glUniformMatrix4fv(glGetUniformLocation(shader, "light_proj"), 1, GL_FALSE, &Window::lightP[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(shader, "light_proj0"), 1, GL_FALSE, &Window::cascadeLightPs[0][0][0]);
	glUniformMatrix4fv(glGetUniformLocation(shader, "light_proj1"), 1, GL_FALSE, &Window::cascadeLightPs[1][0][0]);
	glUniformMatrix4fv(glGetUniformLocation(shader, "light_proj2"), 1, GL_FALSE, &(Window::cascadeLightPs[2])[0][0]);

	glUniformMatrix4fv(glGetUniformLocation(shader, "light_view"), 1, GL_FALSE, &Window::lightV[0][0]);

	glBindVertexArray(VAO);

	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);

	glBindVertexArray(0);

	return false;
}

void Terrain::calcNormals(int v1_idx, int v2_idx, int v3_idx)
{
	// Calculate normal of each vertex as the normalized sum of the normals of the adjacent faces of the vertex
	glm::vec3 v1 = glm::vec3(vertices[v1_idx], vertices[v1_idx + 1], vertices[v1_idx + 2]);
	glm::vec3 v2 = glm::vec3(vertices[v2_idx], vertices[v2_idx + 1], vertices[v2_idx + 2]);
	glm::vec3 v3 = glm::vec3(vertices[v3_idx], vertices[v3_idx + 1], vertices[v3_idx + 2]);

	glm::vec3 e1 = v2 - v1;
	glm::vec3 e2 = v3 - v1;

	glm::vec3 normal = glm::normalize(glm::cross(e1, e2));

	normals[v1_idx] += normal.x;
	normals[v1_idx + 1] += normal.y;
	normals[v1_idx + 2] += normal.z;

	normals[v2_idx] += normal.x;
	normals[v2_idx + 1] += normal.y;
	normals[v2_idx + 2] += normal.z;

	normals[v3_idx] += normal.x;
	normals[v3_idx + 1] += normal.y;
	normals[v3_idx + 2] += normal.z;
}
