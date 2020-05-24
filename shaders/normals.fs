#version 410

in vec4 vNormal;
out vec4 color;

void main()
{
    color.rgb = normalize(vNormal).xyz * vec3(0.5) + vec3(0.5);
    color.a = 1.0;
}

