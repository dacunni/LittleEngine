#version 410

in vec4 vNormal;
out vec4 color;

void main()
{
    color.rgb = normalize(vNormal).xyz * vec3(2.0) - vec3(1.0);
    color.a = 1.0;
}

