#version 430 core
in vec2 UV;
uniform layout(location = 10) vec3 portalColor;


out vec4 color;
uniform layout(binding = 15)  sampler2D texture;


void main()
{
 
  
	color = vec4(portalColor,1);
	


}
