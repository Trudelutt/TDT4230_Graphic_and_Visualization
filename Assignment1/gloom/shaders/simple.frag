#version 430 core
in vec2 UV;
out vec4 color;

//layout(binding = 5) uniform sampler2D myTextureSampler;
uniform sampler2D texSampler;
void main()
{
	//fragmentColor = interColor;
  //color = vec4(0.8f, 0.5f, 0.3f, 1.0f); // make the triangl(es) orange
	color = texture( texSampler, UV );
	//color = texture( myTextureSampler, vec2(0, 0) );
}
