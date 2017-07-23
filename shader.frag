#version 330 core
// This is a sample fragment shader.

// Inputs to the fragment shader are the outputs of the same name from the vertex shader.
// Note that you do not have access to the vertex shader's default output, gl_Position.
in vec3 Normal;
in vec3 SpacePos;
in vec4 LightSpacePos0;
in vec4 LightSpacePos1;
in vec4 LightSpacePos2;
in vec2 TexCoord;
in float ClipSpacePosZ;

// You can output many things. The first vec4 type output determines the color of the fragment
out vec4 color;

uniform sampler2D shadowMap0;
uniform sampler2D shadowMap1;
uniform sampler2D shadowMap2;
uniform sampler2D grassTex;
uniform sampler2D marbleTex;

uniform vec3 dirlight_pos;
uniform vec3 cascadeCutoffs;
uniform float TexID;
uniform float shadowToggle;

float CalcCascadingShadow(float z_pos, vec3 lightDir, vec3 normal);

void main()
{
	vec3 lightDir = normalize(dirlight_pos);
	vec3 normal = normalize(Normal);
	vec3 tex_mat;

	// Get texture
	if(TexID == 0.0)
	{
		tex_mat = texture(grassTex, TexCoord).rgb;
	}
	else
	{
		tex_mat = texture(marbleTex, TexCoord).rgb;
	}

	// Calc shadow value
	float shadow = CalcCascadingShadow(ClipSpacePosZ, lightDir, normal);
											
	// lighting calculations for directional light
	vec3 lightColor = vec3(0.9);
	float diff = max(dot(lightDir, normal), 0.0);

	vec3 diffuse = diff * vec3(0.8, 0.8, 0.8);

	vec3 reflectVec = reflect(-lightDir, normal);
	float spec = pow(max(dot(normal, reflectVec), 0.0), 64.0);
	vec3 specular = spec * vec3(0.8, 0.8, 0.8);
	vec3 ambient = 0.15 * vec3(0.8, 0.8, 0.8);

	// Calculate final color
	vec3 lighting = (1.0 - shadow) * (diffuse + specular + ambient) * tex_mat;
	color = vec4(lighting, 1.0);  
}

float CalcCascadingShadow(float z_pos, vec3 lightDir, vec3 normal)
{
	float shadow = 0.0;
	vec3 projCoords;

	if(z_pos < cascadeCutoffs.x)
	{
		projCoords = LightSpacePos0.xyz / LightSpacePos0.w;
		projCoords = projCoords * 0.5 + 0.5;
		float closestDepth = texture(shadowMap0, projCoords.xy).r;
		float currentDepth = projCoords.z;
		float bias = max(0.05 * (1.0 - dot(normal, lightDir)), 0.005);
		vec2 texelSize = 1.0 / textureSize(shadowMap0, 0);

		for(int x = -1; x <= 1; ++x)
		{
			for(int y = -1; y <= 1; ++y)
			{
				float pcfDepth = texture(shadowMap0, projCoords.xy + vec2(x, y) * texelSize).r; 
				shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;        
			}    
		}

		shadow /= 9.0;
	}
	else if(z_pos < cascadeCutoffs.y)
	{
		projCoords = LightSpacePos1.xyz / LightSpacePos1.w;
		projCoords = projCoords * 0.5 + 0.5;
		float closestDepth = texture(shadowMap1, projCoords.xy).r;
		float currentDepth = projCoords.z;
		float bias = max(0.05 * (1.0 - dot(normal, lightDir)), 0.005);
		vec2 texelSize = 1.0 / textureSize(shadowMap1, 0);

		for(int x = -1; x <= 1; ++x)
		{
			for(int y = -1; y <= 1; ++y)
			{
				float pcfDepth = texture(shadowMap1, projCoords.xy + vec2(x, y) * texelSize).r; 
				shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;        
			}    
		}

		shadow /= 9.0;
	}
	else
	{
		projCoords = LightSpacePos2.xyz / LightSpacePos2.w;
		projCoords = projCoords * 0.5 + 0.5;
		float closestDepth = texture(shadowMap2, projCoords.xy).r;
		float currentDepth = projCoords.z;
		float bias = max(0.05 * (1.0 - dot(normal, lightDir)), 0.005);
		vec2 texelSize = 1.0 / textureSize(shadowMap2, 0);

		for(int x = -1; x <= 1; ++x)
		{
			for(int y = -1; y <= 1; ++y)
			{
				float pcfDepth = texture(shadowMap2, projCoords.xy + vec2(x, y) * texelSize).r; 
				shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;        
			}    
		}

		shadow /= 9.0;
	} 

	if(projCoords.z > 1.0)
		shadow = 0.0;
						
	if(shadowToggle == 0.0)
		shadow = 0.0;

	return shadow;
}

