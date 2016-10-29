#version 410

layout (location=0) in vec4 position;
uniform mat4 projection;
uniform mat4 model_view;

out vec4 vColor;

void main()
{
    gl_Position = projection * model_view * position;
    //vColor.rgb = position.rgb;
    vColor = vec4( 1.0, 0.0, 0.0, 1.0 );
}

