#version 400 core

// Layout

layout (points) in;
layout (points, max_vertices = 1) out;

in vec3 gArea[];

void main() {
    
//    glBegin( GL_TRIANGLE_FAN );
//    glVertex2f(x, y);
//    for( int n = 0; n <= segments; ++n ) {
//        float const t = 2 * M_PI * (float)n / (float)segments;
//        glVertex2f(x + sin(t) * r, y + cos(t) * r);
//    }
//    glEnd();
    
    gl_Position = gl_in[0].gl_Position;
    gl_PointSize = 2.5;
    EmitVertex();
    EndPrimitive();
}

