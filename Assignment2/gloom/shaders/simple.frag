#version 430 core
in vec2 UV;
in vec3 N;
in vec3 surfacePosition;
in mat4 MV;

out vec4 color;

layout(binding = 5) uniform sampler2D texSampler;
layout(binding = 12) uniform sampler2D ambientOcculationTexture;
uniform layout(location = 9) float timeElapsed;
//motion is cameraposition
uniform layout(location = 7) vec3 motion;

void main()
{
  vec3 materialColor = texture( texSampler, UV ).rgb;
  vec3 ambientvector = texture(ambientOcculationTexture, UV).rgb;
  float ambientlight = normalize( (ambientvector.x+ambientvector.y+ ambientvector.z)/3);
  vec3 normal = N;
  float lightIntensity = 1;
  vec3 l = -normalize(vec3(MV*vec4(vec3(cos(timeElapsed)*6,0,sin(timeElapsed)*6),1))-surfacePosition);
 // vec3 l = -normalize(vec3(MV*vec4(10,0,0,1))-surfacePosition);

  //diffuse
	float cosTheta = max(0,dot(normal, l));
	float diffuseCoefficient = 0.3;
	float diffuse = lightIntensity * cosTheta * diffuseCoefficient;

	//specular
	float specularCoefficient = 0.7;
	vec3 r = reflect (-l, normal);
  vec3 surfaceTocamera = normalize(vec3(MV*vec4(motion,1))- surfacePosition);
	float specular = max(0,lightIntensity * specularCoefficient *pow(dot(surfaceTocamera,r),100));
	float ambientColor = 0.5;

	//color = vec4(materialColor*(ambientColor + diffuse + specular),1);
	color = vec4(materialColor*(ambientlight + diffuse + specular),1);
	//color = vec4(ambientvector,1);


}
