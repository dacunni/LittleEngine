#version 410

in vec4 vColor;
out vec4 color;
 
void main()
{
    color = vColor;
    color.a = 0.15;
}

