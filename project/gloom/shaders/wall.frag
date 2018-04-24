#version 430 core
in vec2 UV;


out vec4 color;

layout(binding = 2) uniform sampler2D texSampler;


void main()
{
  vec3 materialColor = texture( texSampler, UV ).rgb;
 
	color = vec4(materialColor,1);


}
