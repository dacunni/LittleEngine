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

uniform vec3 lightPositions[10];
uniform vec3 lightIntensities[10];
uniform int numLights;
 
void main()
{
    vec3 baseColor = vec3(0.5, 0.5, 0.5);

    if(useTexture) {
        baseColor = texture( tex, vUV ).rgb;
    }

    color = vec4(baseColor, 1.0);

    vec3 lighting = vec3(0.0);

    for(int i = 0; i < numLights; i++) {
        vec3 toLight = normalize(lightPositions[i] - vWorldPosition.xyz);
        lighting += lightIntensities[i] * max(dot(vNormal.xyz, toLight), 0);
    }

    color.rgb *= lighting;
}

