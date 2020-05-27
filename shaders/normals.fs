#version 410

in vec4 vNormal;
out vec4 color;

void main()
{
    vec3 normal = normalize(vNormal.xyz);
    color.rgb = normal * vec3(0.5) + vec3(0.5);
    color.a = 1.0;
}

