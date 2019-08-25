#version 430

layout(location = 0) in vec3 vertexIn;            // Each vertex supplied
layout(location = 1) in vec3 normalIn;            // Each normal supplied
layout(location = 2) in vec2 textureCoordinateIn; // Each texture coordinate supplied
layout(location = 3) in vec4 indexes1;            // First 4 bone indexes
layout(location = 4) in vec4 indexes2;            // Second 4 bone indexes
layout(location = 5) in vec4 weights1;            // First 4 bone weights
layout(location = 6) in vec4 weights2;            // Second 4 bone weights

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

uniform mat4 bones[150]; // 150 bones is the maximum bone count

void main() {

    mat4 animationTransform1 = (bones[int(indexes1.x)] * weights1.x) + (bones[int(indexes1.y)] * weights1.y) +
                               (bones[int(indexes1.z)] * weights1.z) + (bones[int(indexes1.w)] * weights1.w);
    mat4 animationTransform2 = (bones[int(indexes2.x)] * weights2.x) + (bones[int(indexes2.y)] * weights2.y) +
                               (bones[int(indexes2.z)] * weights2.z) + (bones[int(indexes2.w)] * weights2.w);
    mat4 animationTransform = animationTransform1 + animationTransform2;
    // The vertex is first transformed by the model and world, then
    // the view/camera and finally the projection matrix
    // The order in which transformation matrices affect the vertex
    // is in the order from right to left
    vec4 transformedVert = projection * view * model * animationTransform * vec4(vertexIn.xyz, 1.0);

    vsData.positionOut = vec3((view * model * animationTransform * vec4(vertexIn.xyz, 1.0))
                                  .xyz); // store only in model space so deferred shadow rendering is done properly

    vsData.normalOut = vec3((normal * animationTransform * vec4(normalIn.xyz, 0.0))
                                .xyz); // Transform normal coordinate in with the normal matrix

    vsData.textureCoordinateOut = textureCoordinateIn; // Passthrough

    // vertex buffer previous
    vsData.prevProjPositionOut = projection * prevView * prevModel * animationTransform * vec4(vertexIn.xyz, 1.0);
    vsData.projPositionOut     = transformedVert; // vertex buffer current

    // Pass the transformed vertex to the fragment shader
    gl_Position = transformedVert;
}