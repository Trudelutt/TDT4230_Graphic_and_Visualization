#version 430 core
in vec2 UV;
in vec3 fragN;
in vec3 surfacePosition;
in mat4 MV;
in vec3 Tfrag;
in vec3 B;
in vec3 N;
in layout(location = 15) mat3 TBN;

out vec4 color;

layout(binding = 5) uniform sampler2D texSampler;
layout(binding = 13) uniform sampler2D ambientOcculationTexture;
layout(binding = 14) uniform sampler2D normalmap;
uniform layout(location = 9) float timeElapsed;
//motion is cameraposition
uniform layout(location = 7) vec3 motion;

void main()
{
  vec3 materialColor = texture( texSampler, UV ).rgb;
  vec3 ambientvector = texture(ambientOcculationTexture, UV).rgb;
  vec3 nm = texture(normalmap, UV).rgb;
  float ambientOcclusionValue = texture(ambientOcculationTexture, UV).r ;
  vec3 nor = normalize(nm *2.0 -1.0);
  vec3 normal = normalize( nor);
  float lightIntensity = 1;
  vec3 l =  normalize(vec3(MV*vec4(vec3(cos(timeElapsed)*6,0,sin(timeElapsed)*6),1))-surfacePosition);
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

	//microshadows
	float aperture = 2.0 * ambientOcclusionValue * ambientOcclusionValue;
	float microshadow = clamp(abs(dot(l, normal)) + aperture - 1.0, 0.0, 1.0);


	//color = vec4(materialColor * microshadow *(0.5 + diffuse + specular),1);
	//color = vec4(materialColor*(0.4 + diffuse + specular),1);
	//color = vec4(materialColor*( microshadow),1);
	color = vec4(TBN[0].xyz,1);
	


}
