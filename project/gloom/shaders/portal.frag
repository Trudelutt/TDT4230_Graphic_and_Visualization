#version 430 core
in vec2 UV;



out vec4 color;
uniform layout(binding = 15)  sampler2D texture;


void main()
{
  vec3 materialColor = texture( texture, UV ).rgb;
  
	color = vec4(materialColor,1.0);
	


}
