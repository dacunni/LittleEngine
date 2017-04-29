#version 410

uniform mat4 world;
uniform mat4 view;
uniform mat4 projection;
in vec4 vObjSpacePosition;
in vec4 vPosition;
in vec4 vWorldPosition;
in vec4 vNormal;
in vec2 vUV;
out vec4 color;

void main()
{
    vec3 eye = normalize(vec3(0.0, 0.0, 1.0)); // FIXME
    vec3 normal = normalize(vNormal.xyz);
    vec3 mirror = normalize(2.0 * dot(eye, normal) * normal - eye);

    // Beckman microfacet distribution function
    const float pi = 3.14159;
    float m = 0.1; // roughness
    vec3 H = normalize(eye + mirror);
    float NdH = max(dot(normal, H), 0);
    float D = 1.0 / (pi * m * m * NdH * NdH * NdH * NdH)
              * exp((NdH * NdH - 1) / (m * m * NdH * NdH));

    color.rgb = vec3(D);
    color.a = 1.0;
}

