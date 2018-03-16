#version 430 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec2 vertexUV;
layout(location = 2) in vec3 normal;
layout(location = 6) in vec3 tangent;
layout(location = 11) in vec3 squareposition;



out vec2 UV;
out vec3 fragN;
out vec3 surfacePosition;
out mat4 MV;



uniform layout(location = 3) mat4x4 MVP;
uniform layout(location = 4) mat4x4 Model;
uniform layout(location = 8) mat4x4 View;
uniform layout(location = 9) float timeElapsed;
uniform layout(location = 10) float vertexCount;
layout(binding = 11) uniform sampler2D texAnimation;
out layout(location = 15)mat3 TBN;

void main()
{
	vec3 T = normalize(vec3(Model *vec4(tangent,   0.0)));
    //vec3 B = normalize(vec3(Model *vec4(bitangent, 0.0)));
	vec3 N = normalize(vec3(Model * vec4(normal,    0.0)));
	T = normalize(T - dot(T, N) * N);
	vec3 B = cross(N, T);
	TBN = mat3(T, B, N);

	MV = Model*View;
	fragN = normalize(transpose(inverse(mat3(MV)))*normal);
	surfacePosition = vec3(MV*vec4(position,1));
	UV = vertexUV;
	float animationOffset = texture(texAnimation,vec2(position.x, timeElapsed)).x;
	gl_VertexID /vertexCount;




	//gl_Position = MVP * vec4(position.xyz + normal*animationOffset*0.05, 1.0f);
	gl_Position = MVP * vec4(position.xyz, 1.0f);
	//gl_Position = MVP * vec4(squareposition.xyz, 1.0f);
}
