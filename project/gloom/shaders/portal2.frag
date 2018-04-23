#version 430 core
in vec2 UV;
in vec3 fragN;
in vec3 surfacePosition;
in mat4 MV;
in layout(location = 15) mat3 TBN;


out vec4 color;
uniform layout(binding = 15)  sampler2D fbo_texture;

uniform layout(location = 9) float timeElapsed;
//motion is cameraposition
uniform layout(location = 7) vec3 motion;
uniform layout(location = 14) mat4 v;

void main()
{
  vec3 materialColor = texture( fbo_texture, UV ).rgb;
  

	//color = vec4(materialColor*( microshadow),1);
	//color = vec4(1.0,1.0,1.0,1.0);
	color = vec4(materialColor,1.0);
	


}
