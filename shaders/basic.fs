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

uniform sampler2D tex;
uniform bool useTexture;

uniform bool highlighted;

uniform vec3 lightPositions[10];
uniform vec3 lightIntensities[10];
uniform int numLights;
 
void main()
{
    if(highlighted) {
        color = vec4(1.0, 1.0, 0.0, 1.0);
        return;
    }

    vec3 normal = normalize(vNormal.xyz);
    vec4 baseColor = vec4(0.5, 0.5, 0.5, 1.0);

    if(useTexture) {
        baseColor = texture( tex, vUV ).rgba;
    }

    color = baseColor;

    if(color.a < 0.01) {
        discard;
    }

    vec3 lighting = vec3(0.0);

    for(int i = 0; i < numLights; i++) {
        vec3 toLight = normalize(lightPositions[i] - vWorldPosition.xyz);
        float NdL = max(dot(normal, toLight), 0);
        lighting += lightIntensities[i] * NdL;
    }

    color.rgb *= lighting;
}

