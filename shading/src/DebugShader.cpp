#include "DebugShader.h"
#include "GLSLShader.h"
#include "HLSLShader.h"
#include "EngineManager.h"

DebugShader::DebugShader(std::string shaderName) {
    if (EngineManager::getGraphicsLayer() == GraphicsLayer::OPENGL) {
        _shader = new GLSLShader(shaderName);
    }
    else {
        _shader = new HLSLShader(shaderName);
    }
}

void DebugShader::runShader(MVP* mvp, 
                            VAO *vao, 
                            std::set<Triangle*> triangleIntersectionList,
                            float* color,
                            GeometryConstruction geometryType) {

    _shader->bind();
    glBindVertexArray(vao->getVAOContext());

    //glUniform mat4 combined model and world matrix, GL_TRUE is telling GL we are passing in the matrix as row major
    _shader->updateData("model", mvp->getModelBuffer());

    //glUniform mat4 view matrix, GL_TRUE is telling GL we are passing in the matrix as row major
    _shader->updateData("view", mvp->getViewBuffer());

    //glUniform mat4 projection matrix, GL_TRUE is telling GL we are passing in the matrix as row major
    _shader->updateData("projection", mvp->getProjectionBuffer());

    //glUniform mat4 projection matrix, GL_TRUE is telling GL we are passing in the matrix as row major
    _shader->updateData("color", color);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    if (geometryType == GeometryConstruction::LINE_WIREFRAME) {
        //Draw lines using the bound buffer vertices at starting index 0 and number of triangles
        glDrawArrays(GL_LINES, 0, (GLsizei)vao->getVertexLength());
    }
    else if (geometryType == GeometryConstruction::TRIANGLE_MESH) {

        //Draw lines using the bound buffer vertices at starting index 0 and number of triangles
        glDrawArrays(GL_TRIANGLES, 0, (GLsizei)vao->getVertexLength());
    }
    else if (geometryType == GeometryConstruction::TRIANGLE_WIREFRAME) {
        std::cout << "Triangle wireframe not here yet!" << std::endl;
    }

    glDisable(GL_BLEND);

    glBindVertexArray(0);
    glUseProgram(0);//end using this shader


    if (triangleIntersectionList.size() > 0) {

        int i = 0;
        auto floatCount = triangleIntersectionList.size() * 3 * 3;
        float* flattenVerts = new float[floatCount];
        for (auto triangle : triangleIntersectionList) {
            auto vertices = triangle->getTrianglePoints();
            auto A = vertices[0];
            auto B = vertices[1];
            auto C = vertices[2];

            flattenVerts[i++] = A.getx();
            flattenVerts[i++] = A.gety();
            flattenVerts[i++] = A.getz();

            flattenVerts[i++] = B.getx();
            flattenVerts[i++] = B.gety();
            flattenVerts[i++] = B.getz();

            flattenVerts[i++] = C.getx();
            flattenVerts[i++] = C.gety();
            flattenVerts[i++] = C.getz();
        }

        //Create a double buffer that will be filled with the vertex data
        GLuint highlightedTrianglesBuffer;
        glGenBuffers(1, &highlightedTrianglesBuffer);
        glBindBuffer(GL_ARRAY_BUFFER, highlightedTrianglesBuffer);
        glBufferData(GL_ARRAY_BUFFER, sizeof(float) * floatCount, flattenVerts, GL_STATIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        //Bind regular vertex, normal and texture coordinate vao
        GLuint highlightedTrianglesVAO;
        glGenVertexArrays(1, &highlightedTrianglesVAO);

        _shader->bind();
        glBindVertexArray(highlightedTrianglesVAO);

        //Bind vertex buff context to current buffer
        glBindBuffer(GL_ARRAY_BUFFER, highlightedTrianglesBuffer);

        //Say that the vertex data is associated with attribute 0 in the context of a shader program
        //Each vertex contains 3 floats per vertex
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

        //Now enable vertex buffer at location 0
        glEnableVertexAttribArray(0);


        //glUniform mat4 combined model and world matrix, GL_TRUE is telling GL we are passing in the matrix as row major
        _shader->updateData("model", Matrix().getFlatBuffer());

        //glUniform mat4 view matrix, GL_TRUE is telling GL we are passing in the matrix as row major
        _shader->updateData("view", mvp->getViewBuffer());

        //glUniform mat4 projection matrix, GL_TRUE is telling GL we are passing in the matrix as row major
        _shader->updateData("projection", mvp->getProjectionBuffer());

        //glUniform mat4 projection matrix, GL_TRUE is telling GL we are passing in the matrix as row major
        float color[] = { 1.0, 0.0, 0.0 };
        _shader->updateData("color", color);


        //Draw lines using the bound buffer vertices at starting index 0 and number of triangles
        glDrawArrays(GL_TRIANGLES, 0, (GLsizei)floatCount / 3);

        glBindVertexArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glUseProgram(0);//end using this shader
    }
}
