#version 440

in vec3 color;


layout(location = 0) out vec4 vFragColor;


void main(void)
{
	vFragColor = vec4(color, 1.0); //Set the final color
}