#version 430

layout(location = 0) in vec3 vertexIn;            // Each vertex supplied
layout(location = 1) in vec3 normalIn;            // Each normal supplied
layout(location = 2) in vec2 textureCoordinateIn; // Each texture coordinate supplied

out VsData {
    vec3 normalOut;            // Transformed normal based on the normal matrix transform
    vec2 textureCoordinateOut; // Passthrough
    vec3 positionOut;          // Passthrough for deferred shadow rendering
    vec4 projPositionOut;
    vec4 prevProjPositionOut;
}
vsData;

uniform mat4 prevModel;  // Previous Model transformation matrix
uniform mat4 prevView;   // Previous View/Camera transformation matrix
uniform mat4 model;      // Model and World transformation matrix
uniform mat4 view;       // View/Camera transformation matrix
uniform mat4 projection; // Projection transformation matrix
uniform mat4 normal;     // Normal matrix

void main() {
    // The vertex is first transformed by the model and world, then
    // the view/camera and finally the projection matrix
    // The order in which transformation matrices affect the vertex
    // is in the order from right to left
    vec4 transformedVert = projection * view * model * vec4(vertexIn.xyz, 1.0);
    // store only in model space so deferred shadow rendering is done properly
    vsData.positionOut = vec3((view * model * vec4(vertexIn.xyz, 1.0)).xyz);
    // Transform normal coordinate in with the normal matrix
    vsData.normalOut            = vec3((normal * vec4(normalIn, 0.0)).xyz);
    vsData.textureCoordinateOut = textureCoordinateIn; // Passthrough
    // vertex buffer previous
    vsData.prevProjPositionOut = projection * prevView * prevModel * vec4(vertexIn.xyz, 1.0);
    vsData.projPositionOut     = transformedVert; // vertex buffer current
    // Pass the transformed vertex to the fragment shader
    gl_Position = transformedVert;
}