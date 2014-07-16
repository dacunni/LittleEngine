#version 410

layout (location=0) in vec4 position;
uniform mat4 projection;
uniform mat4 model_view;

void main()
{
    gl_Position = projection * position;
}

