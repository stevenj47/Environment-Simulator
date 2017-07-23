#include "window.h"

const char* window_title = "CSE 163 Final";
Sphere * sphere;
Sphere * sphere2;
Floor * water_floor;
Terrain * world_terrain;
SkyBox * skybox;
Cube * building1;
Cube * building2;
Cube * building3;

GLint shaderProgram, waterShader, skyboxShader, depthShader, environShader;

#define VERTEX_SHADER_PATH "../shader.vert"
#define FRAGMENT_SHADER_PATH "../shader.frag"

// Default camera parameters
glm::vec3 cam_look_at(0.0f, 0.0f, 0.0f);	
glm::vec3 cam_pos(-4.384f, 353.31f, -203.02f);
glm::vec3 cam_up(0.0f, 1.0f, 0.0f);			
glm::vec3 lightPos = glm::vec3(-100.0f, 300.0f, -100.0f);
glm::vec3 cascadeCutoffs = glm::vec3(100.0f, 500.0f, 1000.0f);
glm::vec4 clip_plane = glm::vec4(0.0f, -1.0f, 0.0f, 30.0f);

float move_factor = 0.0;

int Window::width;
int Window::height;

int MapMode = 0;

glm::mat4 Window::P;
glm::mat4 Window::V;

glm::mat4 Window::lightP;
glm::mat4 Window::cascadeLightPs[3];
glm::mat4 Window::lightV;

bool Window::cullStatus = true;

GLuint depthMapBuff;
GLuint depthMapTex[3];

double lastX, lastY;
bool isLeftPressed = false;

GLuint reflectBuffer, refractBuffer;
GLuint reflectDepth, refractDepth;
GLuint reflectTex, refractTex;
GLuint dudvTex;
GLuint grassTex;
GLuint marbleTex;

const unsigned int REFLECTION_WIDTH = 1024;
const unsigned int REFLECTION_HEIGHT = 1024;

const unsigned int REFRACTION_WIDTH = 1024;
const unsigned int REFRACTION_HEIGHT = 1024;

void Window::initialize_objects()
{	
	shaderProgram = LoadShaders(VERTEX_SHADER_PATH, FRAGMENT_SHADER_PATH);
	waterShader = LoadShaders("../waterShader.vert", "../waterShader.frag");
	skyboxShader = LoadShaders("../skyboxShader.vert", "../skyboxShader.frag");	
	depthShader = LoadShaders("../depthMapShader.vert", "../depthMapShader.frag");
	environShader = LoadShaders("../environShader.vert", "../environShader.frag");
	sphere = new Sphere(shaderProgram);
	sphere2 = new Sphere(shaderProgram);
	world_terrain = new Terrain(0, 0);
	water_floor = new Floor();
	skybox = new SkyBox();
	building1 = new Cube();
	building2 = new Cube();
	building3 = new Cube();

	// Place buildings and terrain
	building1->toWorld = glm::scale(glm::mat4(1.0f), glm::vec3(10.0f, 45.0f, 10.0f)) * glm::translate(glm::mat4(1.0f), glm::vec3(12.0f, 2.3f, 11.0f));
	building2->toWorld = glm::scale(glm::mat4(1.0f), glm::vec3(10.0f, 45.0f, 10.0f)) * glm::translate(glm::mat4(1.0f), glm::vec3(8.0f, 2.5f, 9.0f));
	building3->toWorld = glm::scale(glm::mat4(1.0f), glm::vec3(10.0f, 45.0f, 10.0f)) * glm::translate(glm::mat4(1.0f), glm::vec3(10.0f, 2.5f, 13.0f));

	sphere2->toWorld = glm::scale(glm::mat4(1.0f), glm::vec3(2.0f, 2.0f, 2.0f)) * glm::translate(glm::mat4(1.0f), glm::vec3(50.0f, 70.0f, 54.0f));
	water_floor->toWorld = glm::scale(glm::mat4(1.0f), glm::vec3(2.7f, 1.0f, 2.7f)) * glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 30.0f, 0.0f));
	world_terrain->toWorld = glm::scale(glm::mat4(1.0f), glm::vec3(0.7f, 0.7f, 0.7f)) * glm::translate(glm::mat4(1.0f), glm::vec3(-250.0f, 0.0f, -250.0f));
	
	// Create depth maps
	genDepthMap(1024, 1024);

	// Create water textures
	genWaterBuffers();

	// Create textures
	init_textures();
}

void Window::clean_up()
{
	delete(sphere2);
	delete(sphere);
	delete(world_terrain);
	delete(water_floor);
	delete(skybox);
	delete(building1);
	delete(building2);
	delete(building3);
	glDeleteProgram(shaderProgram);
	glDeleteProgram(waterShader);
	glDeleteProgram(depthShader);
	glDeleteProgram(environShader);
	glDeleteProgram(skyboxShader);
	glDeleteFramebuffers(1, &refractBuffer);
	glDeleteFramebuffers(1, &reflectBuffer);
	glDeleteTextures(1, &reflectTex);
	glDeleteTextures(1, &refractTex);
	glDeleteRenderbuffers(1, &reflectDepth);
	glDeleteTextures(1, &refractDepth);
	glDeleteTextures(1, &dudvTex);
	glDeleteTextures(1, &grassTex);
}

GLFWwindow* Window::create_window(int width, int height)
{
	// Initialize GLFW
	if (!glfwInit())
	{
		fprintf(stderr, "Failed to initialize GLFW\n");
		return NULL;
	}

	// 4x antialiasing
	glfwWindowHint(GLFW_SAMPLES, 4);

#ifdef __APPLE__ // Because Apple hates comforming to standards
	// Ensure that minimum OpenGL version is 3.3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	// Enable forward compatibility and allow a modern OpenGL context
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

	// Create the GLFW window
	GLFWwindow* window = glfwCreateWindow(width, height, window_title, NULL, NULL);

	// Check if the window could not be created
	if (!window)
	{
		fprintf(stderr, "Failed to open GLFW window.\n");
		fprintf(stderr, "Either GLFW is not installed or your graphics card does not support modern OpenGL.\n");
		glfwTerminate();
		return NULL;
	}

	// Make the context of the window
	glfwMakeContextCurrent(window);

	// Set swap interval to 1
	glfwSwapInterval(1);

	// Get the width and height of the framebuffer to properly resize the window
	glfwGetFramebufferSize(window, &width, &height);
	// Call the resize callback to make sure things get drawn immediately
	Window::resize_callback(window, width, height);

	return window;
}

void Window::resize_callback(GLFWwindow* window, int width, int height)
{
	Window::width = width;
	Window::height = height;
	// Set the viewport size. This is the only matrix that OpenGL maintains for us in modern OpenGL!
	glViewport(0, 0, width, height);

	if (height > 0)
	{
		P = glm::perspective(45.0f, (float)width / (float)height, 1.0f, 1000.0f);
		V = glm::lookAt(cam_pos, cam_look_at, cam_up);
	}
}

void Window::idle_callback()
{
	move_factor += 0.001;
	move_factor = fmod(move_factor, 1.0);
}

void Window::display_callback(GLFWwindow* window)
{
	// Render t0 depth maps
	for (int i = 0; i < 3; i++)
	{
		writeDepthMap(1024, 1024, i);
	}

	// Render to water maps
	writeWaterBuffers();

	glViewport(0, 0, width, height);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	// Draw skybox
	skybox->draw(skyboxShader);

	// Render buildings and terrain
	glUseProgram(shaderProgram);
	glActiveTexture(GL_TEXTURE0);
	glUniform1i(glGetUniformLocation(shaderProgram, "shadowMap0"), 0);
	glBindTexture(GL_TEXTURE_2D, depthMapTex[0]);

	glActiveTexture(GL_TEXTURE1);
	glUniform1i(glGetUniformLocation(shaderProgram, "shadowMap1"), 1);
	glBindTexture(GL_TEXTURE_2D, depthMapTex[1]);

	glActiveTexture(GL_TEXTURE2);
	glUniform1i(glGetUniformLocation(shaderProgram, "shadowMap2"), 2);
	glBindTexture(GL_TEXTURE_2D, depthMapTex[2]);

	glActiveTexture(GL_TEXTURE3);
	glUniform1i(glGetUniformLocation(shaderProgram, "grassTex"), 3);
	glBindTexture(GL_TEXTURE_2D, grassTex);

	glActiveTexture(GL_TEXTURE4);
	glUniform1i(glGetUniformLocation(shaderProgram, "marbleTex"), 4);
	glBindTexture(GL_TEXTURE_2D, marbleTex);

	GLuint lightPosUniform = glGetUniformLocation(shaderProgram, "dirlight_pos");
	glUniform3fv(lightPosUniform, 1, &lightPos[0]);	

	GLuint cutoffUniform = glGetUniformLocation(shaderProgram, "cascadeCutoffs");
	glUniform3fv(cutoffUniform, 1, &cascadeCutoffs[0]);

	clip_plane = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);

	GLuint clipPlaneUniform = glGetUniformLocation(shaderProgram, "clip_plane");
	glUniform4fv(clipPlaneUniform, 1, &clip_plane[0]);

	GLuint TexIDLoc = glGetUniformLocation(shaderProgram, "TexID");
	glUniform1f(TexIDLoc, 1.0);

	GLuint ShadowToggleLoc = glGetUniformLocation(shaderProgram, "shadowToggle");
	glUniform1f(ShadowToggleLoc, 1.0);

	building1->render(shaderProgram);
	building2->render(shaderProgram);
	building3->render(shaderProgram);

	glUniform1f(TexIDLoc, 0.0);

    world_terrain->render(shaderProgram);

	// Render water
	glUseProgram(waterShader);

	glUniform1i(glGetUniformLocation(waterShader, "reflectionTexture"), 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, reflectTex);

	glUniform1i(glGetUniformLocation(waterShader, "refractionTexture"), 1);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, refractTex);

	glUniform1i(glGetUniformLocation(waterShader, "dudvMap"), 2);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, dudvTex);

	glUniform1f(glGetUniformLocation(waterShader, "move_fac"), move_factor);

	water_floor->draw(waterShader);

	// Render a sphere with basic environment mapping
	glUseProgram(environShader);

	GLuint camPosUniform = glGetUniformLocation(environShader, "cameraPos");
	glUniform3fv(camPosUniform, 1, &cam_pos[0]);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, skybox->textureID);
	sphere2->render(environShader);

	glfwPollEvents();
	glfwSwapBuffers(window);
}

void Window::key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (action == GLFW_PRESS)
	{
		if (key == GLFW_KEY_ESCAPE)
		{
			glfwSetWindowShouldClose(window, GL_TRUE);
		}
		else if (key == GLFW_KEY_0)
		{
			MapMode = 0;
			fprintf(stderr, "Switched to Settings for Cascading Shadow Map #%i\n", MapMode);
		}
		else if (key == GLFW_KEY_1)
		{
			MapMode = 1;
			fprintf(stderr, "Switched to Settings for Cascading Shadow Map #%i\n", MapMode);
		}
		else if (key == GLFW_KEY_UP)
		{
			if (MapMode == 0 && cascadeCutoffs.x + 10.0 <= 100.0)
			{
				cascadeCutoffs.x += 10.0;
				fprintf(stderr, "Shadow map #0 now applies from a distance 10.0 to %f\n", cascadeCutoffs.x);
			}

			if (MapMode == 1 && cascadeCutoffs.y + 100.0 <= 500.0)
			{
				cascadeCutoffs.y += 100.0;
				fprintf(stderr, "Shadow map #1 now applies from a distance 100.0 to %f\n", cascadeCutoffs.y);
			}
		}
		else if (key == GLFW_KEY_DOWN)
		{
			if (MapMode == 0 && cascadeCutoffs.x - 10.0 > 10.0)
			{
				cascadeCutoffs.x -= 10.0;
				fprintf(stderr, "Shadow map #0 now applies from a distance 10.0 to %f\n", cascadeCutoffs.x);
			}

			if (MapMode == 1 && cascadeCutoffs.y - 100.0 > 100.0)
			{
				cascadeCutoffs.y -= 100.0;
				fprintf(stderr, "Shadow map #1 now applies from a distance 100.0 to %f\n", cascadeCutoffs.y);
			}
		}
	}
}

void Window::mouse_callback(GLFWwindow* window, int button, int action, int mods)
{
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
	{
		double xpos, ypos;
		glfwGetCursorPos(window, &xpos, &ypos);
		lastX = xpos;
		lastY = ypos;
		isLeftPressed = true;
	}

	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE)
	{
		isLeftPressed = false;
	}

}

void Window::scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	float coef = 1.0f;

	if (yoffset > 0)
	{
		coef = -1.0f;
	}

	if (cam_pos.x > 0.01f)
	{
		cam_pos.x += cam_pos.x * coef * 0.1f;
	}
	else if (cam_pos.x < -0.01f)
	{
		cam_pos.x += cam_pos.x * coef * 0.1f;
	}
	else
	{
		// x at 0, do nothing
	}

	if (cam_pos.y > 0.01f)
	{
		cam_pos.y += cam_pos.y * coef * 0.1f;
	}
	else if (cam_pos.y < -0.01f)
	{
		cam_pos.x += cam_pos.y * coef * 0.1f;
	}
	else
	{
		// y at 0, do nothing
	}

	if (cam_pos.z > 0.01f)
	{
		cam_pos.z += cam_pos.z * coef * 0.1f;
	}
	else if (cam_pos.z < -0.01f)
	{
		cam_pos.z += cam_pos.z * coef * 0.1f;
	}
	else
	{
		// z at 0, do nothing
	}

	V = glm::lookAt(cam_pos, cam_look_at, cam_up);
}

void Window::cursor_position_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (isLeftPressed)
	{
		float angle;
		// Perform horizontal (y-axis) rotation
		angle = (float)(lastX - xpos) / 100.0f;
		cam_pos = glm::vec3(glm::rotate(glm::mat4(1.0f), angle, glm::vec3(0.0f, 1.0f, 0.0f)) * glm::vec4(cam_pos, 1.0f));
		cam_up = glm::vec3(glm::rotate(glm::mat4(1.0f), angle, glm::vec3(0.0f, 1.0f, 0.0f)) * glm::vec4(cam_up, 1.0f));
		//Now rotate vertically based on current orientation
		angle = (float)(ypos - lastY) / 100.0f;
		glm::vec3 axis = glm::cross((cam_pos - cam_look_at), cam_up);
		cam_pos = glm::vec3(glm::rotate(glm::mat4(1.0f), angle, axis) * glm::vec4(cam_pos, 1.0f));
		cam_up = glm::vec3(glm::rotate(glm::mat4(1.0f), angle, axis) * glm::vec4(cam_up, 1.0f));
		// Now update the camera
		V = glm::lookAt(cam_pos, cam_look_at, cam_up);

		lastX = xpos;
		lastY = ypos;
	}
}

void Window::genDepthMap(unsigned int s_width, unsigned int s_height)
{
	// Set perspective for each cascading depth map
	cascadeLightPs[0] = glm::ortho(-80.0f, 80.0f, -80.0f, 80.0f, 0.1f, 100.0f);
	cascadeLightPs[1] = glm::ortho(-210.0f, 210.0f, -210.0f, 210.0f, 0.1f, 500.0f);
	cascadeLightPs[2] = glm::ortho(-500.0f, 500.0f, -500.0f, 500.0f, 0.1f, 1000.0f);

	// Create buffer and depth maps
	glGenFramebuffers(1, &depthMapBuff);

	glGenTextures(3, depthMapTex);

	for (int i = 0; i < 3; i++)
	{
		glBindTexture(GL_TEXTURE_2D, depthMapTex[i]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, s_width, s_height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

		GLfloat borderColor[] = { 1.0, 1.0, 1.0, 1.0 };
		glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
	}

	glBindFramebuffer(GL_FRAMEBUFFER, depthMapBuff);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMapTex[0], 0);

	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

}

void Window::writeDepthMap(unsigned int s_width, unsigned int s_height, int idx)
{
	// Renders to depth map at the specified index
	glUseProgram(depthShader);

	glViewport(0, 0, s_width, s_height);

	glBindFramebuffer(GL_FRAMEBUFFER, depthMapBuff);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMapTex[idx], 0);
	glClear(GL_DEPTH_BUFFER_BIT);

	lightP = cascadeLightPs[idx];
	lightV = glm::lookAt(lightPos, cam_look_at, cam_up);

	glCullFace(GL_FRONT);
	sphere2->render(depthShader);
	building1->render(depthShader);
	building2->render(depthShader);
	building3->render(depthShader);

	world_terrain->render(depthShader);
	glCullFace(GL_BACK);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Window::genWaterBuffers()
{
	// Create the reflection buffer and texture
	glGenFramebuffers(1, &reflectBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, reflectBuffer);
	glDrawBuffer(GL_COLOR_ATTACHMENT0);

	glGenTextures(1, &reflectTex);
	glBindTexture(GL_TEXTURE_2D, reflectTex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, REFLECTION_WIDTH, REFLECTION_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, reflectTex, 0);

	glGenRenderbuffers(1, &reflectDepth);
	glBindRenderbuffer(GL_RENDERBUFFER, reflectDepth);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, REFLECTION_WIDTH, REFLECTION_HEIGHT);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, reflectDepth);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// Create refraction buffer and texture
	glGenFramebuffers(1, &refractBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, refractBuffer);
	glDrawBuffer(GL_COLOR_ATTACHMENT0);

	glGenTextures(1, &refractTex);
	glBindTexture(GL_TEXTURE_2D, refractTex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, REFRACTION_WIDTH, REFRACTION_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, refractTex, 0);

	glGenTextures(1, &refractDepth);
	glBindTexture(GL_TEXTURE_2D, refractDepth);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32, REFRACTION_WIDTH, REFRACTION_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, refractDepth, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Window::writeWaterBuffers()
{
	glUseProgram(shaderProgram);
	glEnable(GL_CLIP_DISTANCE0);

	glViewport(0, 0, REFLECTION_WIDTH, REFLECTION_HEIGHT);
	glBindFramebuffer(GL_FRAMEBUFFER, reflectBuffer);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Render to reflection texture, change camera perspective to below water surface, and clip objects below water
	clip_plane = glm::vec4(0.0f, 1.0f, 0.0f, -30.0f);
	GLuint clipPlaneUniform = glGetUniformLocation(shaderProgram, "clip_plane");
	glUniform4fv(clipPlaneUniform, 1, &clip_plane[0]);
	float distance = 2 * (cam_pos.y - 30.0f);
	cam_pos.y -= distance;
	V = glm::lookAt(cam_pos, cam_look_at, -1.0f * cam_up);

	skybox->draw(skyboxShader);

	glUseProgram(shaderProgram);
	GLuint TexIDLoc = glGetUniformLocation(shaderProgram, "TexID");
	glUniform1f(TexIDLoc, 1.0);
	GLuint ShadowToggleLoc = glGetUniformLocation(shaderProgram, "shadowToggle");
	glUniform1f(ShadowToggleLoc, 0.0);

	building1->render(shaderProgram);
	building2->render(shaderProgram);
	building3->render(shaderProgram);

	glUniform1f(TexIDLoc, 0.0);
	world_terrain->render(shaderProgram);

	cam_pos.y += distance;
	V = glm::lookAt(cam_pos, cam_look_at, cam_up);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// Do the same for refraction, but return camera to original position and clip objects above water surface
	glViewport(0, 0, REFRACTION_WIDTH, REFRACTION_HEIGHT);
	glBindFramebuffer(GL_FRAMEBUFFER, refractBuffer);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	clip_plane = glm::vec4(0.0f, -1.0f, 0.0f, 30.0f);
	glUniform4fv(clipPlaneUniform, 1, &clip_plane[0]);

	skybox->draw(skyboxShader);

	glUseProgram(shaderProgram);
	glUniform1f(TexIDLoc, 1.0);
	glUniform1f(ShadowToggleLoc, 0.0);

	building1->render(shaderProgram);
	building2->render(shaderProgram);
	building3->render(shaderProgram);

	glUniform1f(TexIDLoc, 0.0);
	world_terrain->render(shaderProgram);

	V = glm::lookAt(cam_pos, cam_look_at, cam_up);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Window::init_textures()
{
	glGenTextures(1, &dudvTex);
	glBindTexture(GL_TEXTURE_2D, dudvTex);

	// Load dudv map for water
	int img_width, img_height;
	unsigned char* image = SOIL_load_image("textures/waterDUDV.png", &img_width, &img_height, 0, SOIL_LOAD_RGB);

	if (!image)
		printf("Error loading image");

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, img_width, img_height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	SOIL_free_image_data(image);

	glGenTextures(1, &grassTex);
	glBindTexture(GL_TEXTURE_2D, grassTex);

	// Load grass texture for terrain
	image = SOIL_load_image("textures/grassTex.jpg", &img_width, &img_height, 0, SOIL_LOAD_RGB);

	if (!image)
		printf("Error loading image");

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, img_width, img_height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	SOIL_free_image_data(image);

	glGenTextures(1, &marbleTex);
	glBindTexture(GL_TEXTURE_2D, marbleTex);

	// Load marble texture for cubes
	image = SOIL_load_image("textures/marble.jpg", &img_width, &img_height, 0, SOIL_LOAD_RGB);

	if (!image)
		printf("Error loading image");

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, img_width, img_height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	SOIL_free_image_data(image);
}
