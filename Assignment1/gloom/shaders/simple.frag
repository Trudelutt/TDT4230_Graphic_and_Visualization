#version 430 core
in vec2 UV;
in vec3 N;
in vec3 modelposition;
out vec4 color;

layout(binding = 5) uniform sampler2D myTextureSampler;
uniform sampler2D texSampler;
uniform mat4 model;
uniform mat4 view;
void main()
{

	vec3 material = texture( texSampler, UV ).rgb;
	mat3 normalMatrix = transpose(inverse(mat3(model)));
	vec3 normal = normalize(normalMatrix*N);



	//diffuse
	vec3 l = normalize(vec3(0.1,0.1,0.1)-modelposition);
	float cosTheta = clamp(dot(normal, l),0,1);
	vec3 lightColor = vec3(0.6);
	vec3 diffuse = material.rgb* lightColor *cosTheta ;



	vec3 MaterialAmbientColor = vec3(0.6)*material;


//specular
//vec3 incidenceVector = -surfaceToLight;
//vec3 reflectionVector = reflect(incidenceVector, normal);
//vec3 surfaceToCamera = normalize(cameraPosition - surfacePosition);
//float cosAngle = max(0.0, dot(surfaceToCamera, reflectionVector));
//float specularCofficient = pow(cosAngle, materialShininess);
	color = vec4(vec4(MaterialAmbientColor,1.0));
	//color = vec4(diffuse.xyz,1.0);
}
