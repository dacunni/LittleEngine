#version 410

uniform mat4 projection;
uniform mat4 model_view;
in vec4 vPosition;
in vec4 vWorldPosition;
in vec4 vNormal;
out vec4 color;

vec4 light0 = vec4( 10.0, 10.0, 5.0, 1.0 );
 
void main()
{
    vec3 light0Color = vec3( 1, 1, 0 );
    color = vec4(1.0, 0.0, 0.0, 1.0);
    vec4 toLight0 = normalize(light0 - vWorldPosition);
    color.rgb = light0Color * dot(vNormal, toLight0);
}

