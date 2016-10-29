#version 410

uniform float anim_time;
in vec4 vObjSpacePosition;
in vec4 vPosition;
in vec4 vWorldPosition;
in vec4 vNormal;
out vec4 color;

vec4 light0 = vec4( 10.0, 10.0, 5.0, 1.0 );
vec3 light0Color = vec3( 0.5, 0.5, 1.0 );
 
void main()
{
    color = vec4(1.0, 0.0, 0.0, 1.0);
    vec4 toLight0 = normalize(light0 - vWorldPosition);
    color.rgb = light0Color * dot(vNormal, toLight0);

    //float stripe = cos(vWorldPosition.x * 10);
    //float stripe2 = cos(vWorldPosition.y * 10);
    //stripe *= stripe2;
    //stripe = stripe * 0.5 + 0.5;

    vec3 orange = vec3( 1.0, 0.5, 0.0 );
    vec3 yellow = vec3( 1.0, 0.8, 0.5 );
    vec3 blue   = vec3( 0.0, 0.0, 1.0 );

    //vec3 pos = vWorldPosition.xyz;
    float t = anim_time / 10.0;
    //vec3 pos = vObjSpacePosition.xyz + vec3( cos(t), -t, 0 );
    vec3 pos = vObjSpacePosition.xyz + vec3( 0.2 * t, -t, 0 );
    //vec3 pos = cos(vObjSpacePosition.xyz);
    //pos = vec3( cos(pos.x), pos.y, pos.z );
    color.rgb = 2.0 * noise1( pos * 10.0 ) * orange;
    color.rgb += noise1( pos * 20 ) * yellow;
    color.rgb += noise1( pos * 40.0 ) * yellow;

}

