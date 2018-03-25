#version 410

uniform mat4 world;
uniform mat4 view;
uniform mat4 projection;
uniform vec4 cameraPosition;
in vec4 vObjSpacePosition;
in vec4 vPosition;
in vec4 vWorldPosition;
in vec4 vNormal;
in vec2 vUV;
out vec4 color;

vec3 light0 = vec3( 25.0, 25.0, -10.0 );

void main()
{
    vec3 eye = normalize(cameraPosition.xyz - vWorldPosition.xyz);
    vec3 normal = normalize(vNormal.xyz);
    float NdV = max(dot(normal, eye), 0);
    vec3 mirror = normalize(2.0 * NdV * normal - eye);

    vec3 toLight0 = normalize(light0 - vWorldPosition.xyz);
    vec3 H = normalize(eye + toLight0);
    float VdH = max(dot(eye, H), 0);

    // Fresnel: Fraction of light reflected
    float F0 = 0.25; // reflectance at normal incidence
    float F = F0 + (1.0 - F0) * pow(1.0 - VdH, 5);
    color.rgb = vec3(F);
    //color.rgb = H;
    //color.rgb = eye;
    //color.rgb = vec3(VdH);
    //color.rgb = normalize(cameraPosition);
    //color.rgb = vec3(dot(eye, toLight0));
    color.a = 1.0;
}

