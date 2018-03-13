#version 430 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec2 vertexUV;
layout(location = 2) in vec3 normal;

out vec2 UV;
out vec3 N;
out vec3 surfacePosition;
out mat4 MV;


uniform layout(location = 3) mat4x4 MVP;
uniform layout(location = 4) mat4x4 Model;
uniform layout(location = 8) mat4x4 View;
uniform layout(location = 9) float timeElapsed;
uniform layout(location = 10) float vertexCount;
layout(binding = 11) uniform sampler2D texAnimation;


void main()
{
	MV = Model*View;
	N = normalize(transpose(inverse(mat3(MV)))*normal);
	surfacePosition = vec3(MV*vec4(position,1));
	UV = vertexUV;
	float animationOffset = texture(texAnimation,vec2(position.x, timeElapsed)).x;
	gl_VertexID /vertexCount;


	gl_Position = MVP * vec4(position.xyz + normal*animationOffset*0.05, 1.0f);

}
