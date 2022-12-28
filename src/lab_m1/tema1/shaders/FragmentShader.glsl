#version 330

// Input
// TODO(student): Get values from vertex shader
in vec3 frag_color;

// Uniforms
uniform bool IsPoint = false;

// Output
layout(location = 0) out vec4 out_color;


void main()
{
    // TODO(student): Write pixel out color
    out_color = vec4(frag_color, 1);

    if(IsPoint) {
        vec2 circCoord = 2.0 * gl_PointCoord - 1.0;
        if (dot(circCoord, circCoord) > 1.0) {
            discard;
        }
    }
}
