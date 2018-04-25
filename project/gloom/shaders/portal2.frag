#version 430 core
in vec2 UV;



out vec4 color;
uniform layout(location = 7) vec3 portalColor;


void main()
{

color = vec4(portalColor,1.0);
	


}
