#version 330 core

out vec4 color;
  
in vec3 Normal;
in vec3 Position;
in vec2 TexCoords;
in vec4 clipSpace;

uniform sampler2D reflectionTexture;
uniform sampler2D refractionTexture;
uniform sampler2D dudvMap;

uniform float move_fac;

const float wave_mult = 0.02;
void main()
{   
    // Calculate screen space texture coordinates
	vec2 ndc = (clipSpace.xy / clipSpace.w) / 2.0 + 0.5;
	vec2 refractTexCoords = ndc;
	vec2 reflectTexCoords = vec2(-ndc.x, ndc.y);

	// wave mult controls distortion strength, move_fac controls ripple speed, 
	vec2 distort1 = (texture(dudvMap, vec2(TexCoords.x + move_fac, TexCoords.y)).rg * 2.0 - 1.0) * wave_mult;
	vec2 distort2 = (texture(dudvMap, vec2(-TexCoords.x + move_fac, TexCoords.y + move_fac)).rg * 2.0 - 1.0) * wave_mult;
	vec2 sum_distort = distort1 + distort2;

	// Add distortion + ripple effect to offset both texture corrdinates
	refractTexCoords += sum_distort;
	reflectTexCoords += sum_distort;

	vec4 reflectColour = texture(reflectionTexture, reflectTexCoords);
	vec4 refractColour = texture(refractionTexture, refractTexCoords);

	// Mix to create result
	color = mix(reflectColour, refractColour, 0.5);

	// Mix with light blue color so water is not completely clear
	color = mix(color, vec4(0.0, 0.3, 0.5, 1.0), 0.15);
}  