
#version 330 core
  
layout (location = 0) in vec3 position;


out vec4 col;

void main()
{
    gl_Position = vec4(position, 1.0);
	col = vec4(0,0,1,1);
}