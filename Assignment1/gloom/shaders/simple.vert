#version 430 core

layout(location = 0) in vec3 position;
layout(location = 2) in vec2 vertexUV;

out vec2 UV;
uniform layout(location = 3) mat4x4 MVP;


void main()
{
	mat4x4 matrix = mat4(1);
	// for task 3
	// matrix[0] = vec4(1,0,0,0);
	// matrix[1] = vec4(0,1,0,0.5);
	//interColor = color;
	UV = vertexUV;
    //gl_Position = MVP* vec4(position, 1.0f)*matrix;
	gl_Position = MVP * vec4(position.x, position.y, position.z, 1.0f);

}
