#version 410

layout (location=0) in vec4 position;
uniform mat4 world;
uniform mat4 view;
uniform mat4 projection;

out vec4 vColor;

void main()
{
    gl_Position = projection * view * world * position;
    gl_PointSize = 2.0;
    //vColor.rgb = position.rgb;
    vColor = vec4( 1.0, 0.0, 0.0, 1.0 );
}

