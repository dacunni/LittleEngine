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

vec3 light0 = vec3( 25.0, 25.0, 0.0 );

void main()
{
    vec3 eye = normalize(vec3(0.0, 0.0, 1.0)); // FIXME
    vec3 normal = normalize(vNormal.xyz);
    vec3 mirror = normalize(2.0 * dot(eye, normal) * normal - eye);

    vec3 toLight0 = normalize(light0 - vWorldPosition.xyz);
    vec3 H = normalize(eye + toLight0);

    float NdH = max(dot(normal, H), 0);
    float NdV = max(dot(normal, eye), 0);
    float VdH = max(dot(eye, H), 0);
    float NdL = max(dot(normal, toLight0), 0);
    float LdH  = max(dot(toLight0, H), 0);

    // Geometric attenuation according to Blinn
    float G1 = 2.0 * NdH * NdV / VdH;
    float G2 = 2.0 * NdH * NdL / LdH;
    float G = min(1.0, min(G1, G2));

    color.rgb = vec3(G);
    color.a = 1.0;
}

