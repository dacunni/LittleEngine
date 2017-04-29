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

vec3 light0 = vec3( 10.0, 0.0, 5.0 );
vec3 light0Color = vec3( 0.5, 0.5, 1.0 );
vec3 light1 = vec3( -10.0, 10.0, 5.0 );
vec3 light1Color = vec3( 0.8, 0.7, 0.0 );
 
void main()
{
#if 0
    // TODO - handle textured or not
    vec3 baseColor = texture( tex, vUV ).rgb;
#else
    vec3 baseColor = vec3(0.5, 0.5, 0.5);
#endif

    //color = vec4(baseColor, 1.0);

    vec3 eye = normalize(vec3(0.0, 0.0, 1.0)); // FIXME
    vec3 normal = normalize(vNormal.xyz);
    vec3 mir = normalize(2.0 * dot(eye, normal) * normal - eye);

    vec3 lighting = vec3(0.0);
    vec3 toLight0 = normalize(light0 - vWorldPosition.xyz);
    vec3 toLight1 = normalize(light1 - vWorldPosition.xyz);
    //vec3 toLight1 = vec3(0.0, 1.0, 0.0);

    // Diffuse
    lighting += light0Color * baseColor * max(dot(vNormal.xyz, toLight0), 0);
    lighting += light1Color * baseColor * max(dot(vNormal.xyz, toLight1), 0);
    // Specular
    lighting += light0Color * 1.0 * pow(max(dot(mir, toLight0), 0), 15.0);
    lighting += light1Color * 1.0 * pow(max(dot(mir, toLight1), 0), 15.0);
    //lighting += mir;

    //color.rgb *= lighting;
    color.rgb = lighting;
    
    color.a = 1.0;
}

