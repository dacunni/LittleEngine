#version 410

uniform mat4 world;
uniform mat4 view;
uniform mat4 projection;
uniform float anim_time;
in vec4 vObjSpacePosition;
in vec4 vPosition;
in vec4 vWorldPosition;
in vec4 vNormal;
in vec2 vUV;
out vec4 color;
uniform sampler2D tex;
uniform bool useTexture;

const float pi = 3.14159;

vec3 light0 = vec3( 25.0, 25.0, 0.0 );
//vec3 light0 = vec3( 15.0, 15.0, 20.0 * cos(anim_time) );
vec3 light0Color = vec3( 0.8, 0.7, 0.5 );
//vec3 light0Color = vec3(1.0); // white

void main()
{
    vec3 cameraPosition = -view[3].xyz; // FIXME - Is this right?
    vec3 eye = normalize(cameraPosition - vWorldPosition.xyz);
    vec3 normal = normalize(vNormal.xyz);

    vec3 toLight0 = normalize(light0 - vWorldPosition.xyz);

    vec3 H = normalize(eye + toLight0);

    float NdH = max(dot(normal, H), 0);
    float NdV = max(dot(normal, eye), 0);
    float VdH = max(dot(eye, H), 0);
    float NdL = max(dot(normal, toLight0), 0);

    vec3 specular = vec3(0.0);

    if(NdL > 0.0 && NdV > 0.0f) {
        // Fresnel: Fraction of light reflected
        float F0 = 0.25; // reflectance at normal incidence
        float F = F0 + (1.0 - F0) * pow(1.0 - VdH, 5);

        // Beckman microfacet distribution function
        // roughness
        float m = 0.1;
        //float m = cos(anim_time) * 0.5 + 0.5; m = min(max(m, 0.01), 1.0);
        float D = 1.0 / (pi * m * m * NdH * NdH * NdH * NdH)
            * exp((NdH * NdH - 1) / (m * m * NdH * NdH));

        // Geometric attenuation according to Blinn
        float G1 = 2.0 * NdH * NdV / VdH;
        float G2 = 2.0 * NdH * NdL / VdH;
        float G = min(1.0, min(G1, G2));

        specular = vec3(F * D * G / (pi * NdL * NdV));
        specular *= light0Color;
    }

    vec3 Ka = vec3(0.3);
    vec3 Kd = vec3(0.5);
    if(useTexture) {
        Kd = texture( tex, vUV ).rgb;
    }

    vec3 ambient = Ka * light0Color;
    vec3 diffuse = Kd * light0Color;

    color.rgb = ambient + NdL * (diffuse + (1.0 - Kd) * specular);
    color.a = 1.0;
}

