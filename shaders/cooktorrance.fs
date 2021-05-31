#version 410

uniform mat4 world;
uniform mat4 view;
uniform mat4 projection;
uniform vec4 cameraPosition;
uniform float roughness;
uniform float F0;
uniform float anim_time;
uniform sampler2D tex;
uniform bool useTexture;
uniform bool highlighted;

in vec4 vObjSpacePosition;
in vec4 vPosition;
in vec4 vWorldPosition;
in vec4 vNormal;
in vec2 vUV;
out vec4 color;

struct PointLight {
    vec3 position;
    vec3 color;
};

uniform vec3 lightPositions[10];
uniform vec3 lightIntensities[10];
uniform int numLights;

const float pi = 3.14159;

float specularReflection(vec3 worldPos, vec3 eye, vec3 normal, PointLight light) {
    vec3 toLight = normalize(light.position - worldPos);
    vec3 H = normalize(eye + toLight);
    float NdH = max(dot(normal, H), 0);
    float NdV = max(dot(normal, eye), 0);
    float VdH = max(dot(eye, H), 0);
    float NdL = max(dot(normal, toLight), 0);

    if(NdL <= 0.0 || NdV < 0.0) {
        return 0.0;
    }

    // Fresnel: Fraction of light reflected
    float F = F0 + (1.0 - F0) * pow(1.0 - VdH, 5);

    // Beckman microfacet distribution function
    // roughness
    float m = roughness;
    //float m = cos(anim_time) * 0.5 + 0.5; m = min(max(m, 0.01), 1.0);
    float D = 1.0 / (pi * m * m * NdH * NdH * NdH * NdH)
        * exp((NdH * NdH - 1) / (m * m * NdH * NdH));

    // Geometric attenuation according to Blinn
    float G1 = 2.0 * NdH * NdV / VdH;
    float G2 = 2.0 * NdH * NdL / VdH;
    float G = min(1.0, min(G1, G2));

    return F * D * G / (pi * NdL * NdV);
}

void main()
{
    if(highlighted) {
        color = vec4(1.0, 1.0, 0.0, 1.0);
        return;
    }

    vec3 eye = normalize(cameraPosition.xyz - vWorldPosition.xyz);
    vec3 normal = normalize(vNormal.xyz);
    vec3 worldPos = vWorldPosition.xyz;
    color.rgb = vec3(0.0);

    for(int i = 0; i < numLights; i++) {
        PointLight light = PointLight(lightPositions[i], lightIntensities[i]);
        vec3 toLight = light.position - worldPos;
        float dist = length(toLight);
        float distSq = dist * dist;
        float NdL = max(dot(normal, normalize(toLight)), 0);

        vec3 Ka = vec3(0.0);
        vec3 Kd = vec3(0.3);
        if(useTexture) {
            Kd = texture( tex, vUV ).rgb;
        }

        vec3 Li = light.color / distSq;
        vec3 ambient = Ka * Li;
        vec3 diffuse = Kd * Li;
        vec3 specular = vec3(specularReflection(worldPos, eye, normal, light)) * Li;
        color.rgb += ambient + NdL * (diffuse + (1.0 - Kd) * specular);
    }

    color.a = 1.0;
}

