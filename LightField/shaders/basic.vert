#version 440

uniform mat4 mProjection, mModelView;

layout(location = 0) in vec2 vVertex;
layout(location = 3) in vec2 vTexture;

out vec2 texPos;

void main()
{
	texPos = vTexture;
	gl_Position = mProjection * mModelView * vec4(vVertex, 0.0f,1.0f);
}