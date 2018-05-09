#version 330

uniform mat4 view;		 // View/Camera transformation matrix
uniform mat4 projection; // Projection transformation matrix

out vec3 vsViewDirection;
 
void main() {
    gl_Position = vec4(((gl_VertexID & 1) << 2) - 1, (gl_VertexID & 2) * 2 - 1, 0.0, 1.0);
    vsViewDirection = mat3(view) * (projection * gl_Position).xyz;
}