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
    color = vec4(1.0, 1.0, 1.0, 1.0);

    if(useTexture) {
        color.rgb = texture( tex, vUV ).rgb;
    }
}


