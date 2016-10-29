#version 410

uniform float anim_time;
in vec4 vObjSpacePosition;
in vec4 vPosition;
in vec4 vWorldPosition;
in vec4 vNormal;
out vec4 color;

float t = anim_time / 10.0;
vec4 light0 = vec4( 10.0, 10.0, 5.0, 1.0 );
vec3 light0Color = vec3( 0.5, 0.5, 1.0 );
 
void main()
{
    color = vec4(1.0, 0.0, 0.0, 1.0);
    vec4 toLight0 = normalize(light0 - vWorldPosition);
    color.rgb = light0Color * dot(vNormal, toLight0);

    vec3 brown = vec3( 0.545, 0.271, 0.075 );
    vec3 black = vec3( 0.0, 0.0, 0.0 );

    vec3 pos = vObjSpacePosition.xyz;
    pos += noise3(pos);
    pos.xy *= 4;
    float dist = sqrt(dot(pos, pos)) * 20;

    color.rgb = mix(brown, black, fract(cos(dist) * 0.5 + 0.5));
}

