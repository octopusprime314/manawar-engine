#version 430

out VsData {
    vec2 texCoordOut;
    vec3 positionOut;
}
vsData;

uniform int fireType;
uniform mat4 model;      // Model view matrix
uniform mat4 view;       // View matrix
uniform mat4 projection; // projection matrix
uniform mat4 inverseViewNoTrans;

void main() {

    // there are five possible fire types in the pixel shader
    // choosing which fire type is in intervals of 0.2 in the u coordinate
    // and the v coordinates will always be between 0 and 0.5 for now
    float minU   = fireType * 0.2f;
    float maxU   = minU + 0.2f;
    vec3  vertex = vec3(0.0);

    if (gl_VertexID == 0) {
        vertex             = vec3(-1.0, 1.0, 0.0);
        vsData.texCoordOut = vec2(minU, 0.5);
    } else if (gl_VertexID == 1) {
        vertex             = vec3(-1.0, -1.0, 0.0);
        vsData.texCoordOut = vec2(minU, 0.0);
    } else if (gl_VertexID == 2) {
        vertex             = vec3(1.0, 1.0, 0.0);
        vsData.texCoordOut = vec2(maxU, 0.5);
    } else if (gl_VertexID == 3) {
        vertex             = vec3(1.0, -1.0, 0.0);
        vsData.texCoordOut = vec2(maxU, 0.0);
    }

    // The vertex is first transformed by the model and world, then
    // the view/camera and finally the projection matrix
    // The order in which transformation matrices affect the vertex
    // is in the order from right to left
    vec4 transformedVert = projection * view * model * inverseViewNoTrans * vec4(vertex.xyz, 1.0);
    gl_Position          = transformedVert;

    vsData.positionOut = vec3((view * model * inverseViewNoTrans * vec4(vertex.xyz, 1.0)).xyz);
}