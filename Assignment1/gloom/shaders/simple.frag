#version 430 core
in vec2 UV;
in vec3 N;
in mat4 modelposition;
in vec3 surfacePosition;
in mat4 MV;

out vec4 color;

layout(binding = 5) uniform sampler2D myTextureSampler;
uniform sampler2D texSampler;
uniform mat4 model;
uniform mat4 view;
uniform layout(location = 7) vec3 motion;

void main()
{
  vec3 material = texture( texSampler, UV ).rgb;
	mat3 normalMatrix = transpose(inverse(mat3(modelposition)));
	vec3 surfacePosition_wordspace = (MV *vec4(surfacePosition,1)).xyz;
	vec3 normal = normalize(normalMatrix*N);
	vec3 light =  vec3(0,0,-300)-surfacePosition;
	light = (MV * vec4(light,1)).xyz;
	vec3 l = normalize(light);

	//diffuse
	float cosTheta = max(0,dot(normal, l));
	float lightColor = 0.8;
	vec3 diffuse = cosTheta * lightColor*material;

	//specular
	float specular_cofficient = 0.8;
	vec3 r = reflect (l, normal);
	vec3 specular = material.rgb * specular_cofficient *pow(dot(r,normalize(motion)),100);
	vec3 MaterialAmbientColor = vec3(0.4)*material;


	//color = vec4(vec4(MaterialAmbientColor,1));
	//color = vec4(vec4(MaterialAmbientColor,1))+ vec4(diffuse.xyz,1.0) + vec4(specular,1);
	//color = texture( texSampler, UV );
	//color = vec4(diffuse,1);
	color = vec4(l,1);

}
