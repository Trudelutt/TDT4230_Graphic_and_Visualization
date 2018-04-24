#version 430 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec2 vertexUV;





out vec2 UV;



uniform layout(location = 5) mat4x4 MVP;


void main()
{

	UV = vertexUV;

	gl_Position = MVP * vec4(position.xyz, 1.0f);

}
