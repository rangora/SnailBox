#version 460 core
layout(location = 0) in vec3 aPos;

uniform mat4 tranform;

void main()
{
    gl_Position = tranform * vec4(aPos, 1.0);
}