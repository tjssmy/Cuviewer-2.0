#version 130

in vec4 vertex;
in vec4 colour;
out vec4 colourOut;

void main( void )
{
    gl_Position = vertex;
    colourOut = colour;
}
