#version 440

layout(binding = 0) uniform sampler3D LightField;

layout(location = 0) out vec4 vFragColor;

uniform int corteX = 0;
uniform int corteY = 0;

in vec2 texPos;

void main(void)
{

	//Set the final color
	vFragColor = vec4(texture(LightField, vec3(texPos.xy, (corteY * 16 + corteX + 0.5)/(16*16))).rgb, 1.0); 
	//vFragColor = vec4(texPos.xy, 0.0, 1.0); 
}