#version 410

in vec2 vUV;
out vec4 color;
 
void main()
{
    color = vec4(vUV, 0.0, 1.0);
}

