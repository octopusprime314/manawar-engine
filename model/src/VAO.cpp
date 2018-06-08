#include "VAO.h"
#include "Model.h"

VAO::VAO(){

}
VAO::~VAO(){

}

void VAO::setVertexContext(GLuint context){
    _vertexBufferContext = context;
}

void VAO::setNormalContext(GLuint context){
    _normalBufferContext = context;
}

void VAO::setTextureContext(GLuint context){
    _textureBufferContext = context;
}

void VAO::setNormalDebugContext(GLuint context){
    _debugNormalBufferContext = context;
}

GLuint VAO::getVertexContext() {
    return _vertexBufferContext;
}

GLuint VAO::getNormalContext() {
    return _normalBufferContext;
}

GLuint VAO::getTextureContext() {
    return _textureBufferContext;
}

GLuint VAO::getNormalDebugContext() {
    return _debugNormalBufferContext;
}

void VAO::createVAO(RenderBuffers* renderBuffers, ModelClass classId, Animation* animation)
{
    assert(classId != ModelClass::AnimatedModelType &&
           "Animated rendering is currently broken. "
           "You should fix this function, because something is loading animation data.");

    std::vector<Vector4>& vertices     = *renderBuffers->getVertices();
    std::vector<Vector4>& normals      = *renderBuffers->getNormals();
    std::vector<Tex2>&    textures     = *renderBuffers->getTextures();
    std::vector<int>&     indices      = *renderBuffers->getIndices();
    std::vector<Vector4>& debugNormals = *renderBuffers->getDebugNormals();

    const size_t vertexCount = indices.size();
    if (vertexCount == 0) {
        printf("Creating a model with no vertices!");
    }

    // If no indices were set, create a dumb index buffer: 0, 1, 2, 3, etc.
    if (indices.empty()) {
        indices.reserve(vertexCount);
        for (int i = 0; i < vertexCount; i += 1) {
            indices.emplace_back(i);
        }
    }

    if (normals.size() != vertexCount) {
        printf("(vao=%p) Resizing normals      from %zu to %zu.\n", this, normals.size(), vertexCount);
        normals.resize(vertexCount);
    }
    if (textures.size() != vertexCount) {
        printf("(vao=%p) Resizing textures     from %zu to %zu.\n", this, textures.size(), vertexCount);
        textures.resize(vertexCount);
    }
    if (debugNormals.size() != vertexCount) {
        printf("(vao=%p) Resizing debugNormals from %zu to %zu.\n", this, debugNormals.size(), vertexCount);
        debugNormals.resize(vertexCount);
    }

    // Generate buffers
    {
        // Indices
        glGenBuffers(1, &_indexBufferContext);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _indexBufferContext);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                     indices.size() * sizeof(float),
                     indices.data(),
                     GL_STATIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

        // Positions
        glGenBuffers(1, &_vertexBufferContext);
        glBindBuffer(GL_ARRAY_BUFFER, _vertexBufferContext);
        glBufferData(GL_ARRAY_BUFFER,
                     vertexCount * sizeof(float),
                     vertices.data(),
                     GL_STATIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        // Normals
        glGenBuffers(1, &_normalBufferContext);
        glBindBuffer(GL_ARRAY_BUFFER, _normalBufferContext);
        glBufferData(GL_ARRAY_BUFFER,
                     vertexCount * sizeof(float),
                     normals.data(),
                     GL_STATIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        // Texture Coords
        glGenBuffers(1, &_textureBufferContext); // Do not need to double buffer
        glBindBuffer(GL_ARRAY_BUFFER, _textureBufferContext);
        glBufferData(GL_ARRAY_BUFFER,
                     vertexCount * sizeof(float),
                     textures.data(),
                     GL_STATIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        // Debug Normals (unused?)
        glGenBuffers(1, &_debugNormalBufferContext);
        glBindBuffer(GL_ARRAY_BUFFER, _debugNormalBufferContext);
        glBufferData(GL_ARRAY_BUFFER,
                     vertexCount * sizeof(float),
                     debugNormals.data(),
                     GL_STATIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    // Setup VAOs for the Deferred Pass
    {
        // Bind regular vertex, normal and texture coordinate vao
        glGenVertexArrays(1, &_vaoContext);
        glBindVertexArray(_vaoContext);

        // Indices
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _indexBufferContext);

        // Bind vertex buff context to current buffer
        glBindBuffer(GL_ARRAY_BUFFER, _vertexBufferContext);
        // Note: Stride here is important - Vector4 has extra data that we don't pass to OpenGL, so we
        //       need to specifiy the actual size of our 4D vectors.
        //       (We ignore the w component.)
        //       This happens on all of these, but I'll only write it out on this one. ;)
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vector4), 0);
        glEnableVertexAttribArray(0);

        // Bind normal buff context to current buffer
        glBindBuffer(GL_ARRAY_BUFFER, _normalBufferContext);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vector4), 0);
        glEnableVertexAttribArray(1);

        // Bind texture coordinate buff context to current buffer
        glBindBuffer(GL_ARRAY_BUFFER, _textureBufferContext);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vector4), 0);
        glEnableVertexAttribArray(2);

        // Close vao
        glBindVertexArray(0);
    }

    // Setup VAOs for Shadow Passes
    {
        glGenVertexArrays(1, &_vaoShadowContext);
        glBindVertexArray(_vaoShadowContext);

        // Indices
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _indexBufferContext);

        // Shadows only need position data.
        glBindBuffer(GL_ARRAY_BUFFER, _vertexBufferContext);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vector4), 0);
        glEnableVertexAttribArray(0);

        // Close vao
        glBindVertexArray(0);
    }
}

GLuint VAO::getVAOContext() {
    return _vaoContext;
}

GLuint VAO::getVAOShadowContext() {
    return _vaoShadowContext;
}
