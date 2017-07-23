#version 330 core
// NOTE: Do NOT use any version older than 330! Bad things will happen!

// This is an example vertex shader. GLSL is very similar to C.
// You can define extra functions if needed, and the main() function is
// called when the vertex shader gets run.
// The vertex shader gets called once per vertex.

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 tex_coords;

// Uniform variables can be updated by fetching their location and passing values to that location
uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

uniform mat4 light_proj0;
uniform mat4 light_proj1;
uniform mat4 light_proj2;

uniform mat4 light_view;
uniform vec4 clip_plane;

// Outputs of the vertex shader are the inputs of the same name of the fragment shader.
// The default output, gl_Position, should be assigned something. You can define as many
// extra outputs as you need.
out vec3 Normal;
out vec3 SpacePos;
out vec4 LightSpacePos0;
out vec4 LightSpacePos1;
out vec4 LightSpacePos2;
out float ClipSpacePosZ;
out vec2 TexCoord;

void main()
{
    // OpenGL maintains the D matrix so you only need to multiply by P, V (aka C inverse), and M
    gl_Position = projection * view * model * vec4(position, 1.0);
	Normal = transpose(inverse(mat3(model))) * normal;
	SpacePos = vec3(model * vec4(position, 1.0));
	TexCoord = tex_coords;

	LightSpacePos0 = light_proj0 * light_view * model * vec4(position, 1.0);
	LightSpacePos1 = light_proj1 * light_view * model * vec4(position, 1.0);
	LightSpacePos2 = light_proj2 * light_view * model * vec4(position, 1.0);

	ClipSpacePosZ = gl_Position.z;
	gl_ClipDistance[0] = dot(model * vec4(position, 1.0), clip_plane);
}
