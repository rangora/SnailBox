#version 460 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNomral;
layout(location = 2) in vec2 aTexCoord;

layout(std140) uniform MatrixBlock
{
    mat4 _transform;
    mat4 _modelTransform;
};

out vec3 normal;
out vec3 position;
out vec2 texCoord;

void main()
{
    gl_Position = _transform * vec4(aPos, 1.f);
    normal = (transpose(inverse(_modelTransform)) * vec4(aNomral, 0.f)).xyz;
    texCoord = aTexCoord;
    position = (_modelTransform * vec4(aPos, 1.f)).xyz;
}