#version 400 core

// Layout

layout (points) in;
layout (points, max_vertices = 1) out;

in vec3 gNormal[];
in float gArea[];

void main() {
    
//    glBegin( GL_TRIANGLE_FAN );
//    glVertex2f(x, y);
//    for( int n = 0; n <= segments; ++n ) {
//        float const t = 2 * M_PI * (float)n / (float)segments;
//        glVertex2f(x + sin(t) * r, y + cos(t) * r);
//    }
//    glEnd();
    
//    vec4 displacement = vec4(normalize(gNormal[0]) * 0.001, 0.0);
    gl_Position = gl_in[0].gl_Position;// + displacement;
    
    gl_PointSize = 5.0;
    EmitVertex();
    EndPrimitive();
}
