#version 410

layout (location=0) in vec4 position;
layout (location=1) in vec4 normal;
uniform mat4 projection;
uniform mat4 model_view;
out vec4 vPosition;
out vec4 vWorldPosition;
out vec4 vNormal;

void main()
{
    vWorldPosition = model_view * position;
    vPosition = projection * vWorldPosition;
    gl_Position = vPosition;
    vNormal = projection * model_view * normalize(normal);
}

