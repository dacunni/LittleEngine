#version 410

uniform mat4 world;
uniform mat4 view;
uniform mat4 projection;
in vec4 vObjSpacePosition;
in vec4 vPosition;
in vec4 vWorldPosition;
in vec4 vNormal;
out vec4 color;

vec4 light0 = vec4( 10.0, 10.0, 5.0, 1.0 );
vec3 light0Color = vec3( 0.5, 0.5, 1.0 );
 
void main()
{
    color = vec4(0.0, 1.0, 0.0, 1.0);
    vec4 toLight0 = normalize(light0 - vWorldPosition);
    //color.rgb = light0Color * dot(vNormal, toLight0);
    //color = normalize(vNormal); // visualize normal vector
    color.g *= 0.25 + 0.75 * 0.5 * (cos(vWorldPosition.x) * cos(vWorldPosition.z) + 1.0);
}

