#version 430 core
in vec2 UV;
out vec3 color;

uniform sampler2D myTextureSampler;

void main()
{			
	//fragmentColor = interColor;
    //color = vec4(0.8f, 0.5f, 0.3f, 1.0f); // make the triangl(es) orange
	color = texture( myTextureSampler, UV ).rgb;
}
