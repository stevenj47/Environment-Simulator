#version 330 core
out vec4 color;

in vec3 Normal;
in vec3 Position;

uniform vec3 cameraPos;
uniform samplerCube skybox;

void main()
{             
    vec3 dir = normalize(Position - cameraPos);
    vec3 refl = reflect(dir, normalize(Normal));
    color = vec4(texture(skybox, refl).rgb, 1.0);
}