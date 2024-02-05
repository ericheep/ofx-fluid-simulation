#version 410

// contrast
// vertex shader

// default variables for a vertex shader
uniform mat4 modelViewProjectionMatrix;
uniform mat4 modelViewMatrix;
uniform mat4 projectionMatrix;
uniform mat4 textureMatrix;

in vec4 position;
in vec4 color;
in vec4 normal;
in vec2 texcoord;

out vec2 texCoordVarying;

void main()
{
    // here we move the texture coordinates
    texCoordVarying = texcoord;
    
    // send the vertices to the fragment shader
    gl_Position = modelViewProjectionMatrix * position;
}
