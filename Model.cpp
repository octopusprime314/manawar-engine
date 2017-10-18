
#include "Model.h"
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "glm\gtc\matrix_transform.hpp"

Model::Model() {

	//Load default shader
	_shaderProgram.compile();

    //DEFAULT MODEL
    //Cube vertices modeled around origin (0,0,0) with length,width,height of 2

    //xy plane triangle top + z
    _vertices.push_back(Vector4(1.0, 1.0, 1.0, 1.0));
    _vertices.push_back(Vector4(-1.0, -1.0, 1.0, 1.0));
    _vertices.push_back(Vector4(-1.0, 1.0, 1.0, 1.0));

    //xy plane triangle bottom + z
    _vertices.push_back(Vector4(1.0, 1.0, 1.0, 1.0));
    _vertices.push_back(Vector4(-1.0, -1.0, 1.0, 1.0));
    _vertices.push_back(Vector4(1.0, -1.0, 1.0, 1.0));

    //xy plane triangle top - z
    _vertices.push_back(Vector4(1.0, 1.0, -1.0, 1.0));
    _vertices.push_back(Vector4(-1.0, -1.0, -1.0, 1.0));
    _vertices.push_back(Vector4(-1.0, 1.0, -1.0, 1.0));

    //xy plane triangle bottom - z
    _vertices.push_back(Vector4(1.0, 1.0, -1.0, 1.0));
    _vertices.push_back(Vector4(-1.0, -1.0, -1.0, 1.0));
    _vertices.push_back(Vector4(1.0, -1.0, -1.0, 1.0));

    //zy plane triangle top + x
    _vertices.push_back(Vector4(1.0, 1.0, 1.0, 1.0));
    _vertices.push_back(Vector4(1.0, -1.0, 1.0, 1.0));
    _vertices.push_back(Vector4(1.0, -1.0, -1.0, 1.0));

    //zy plane triangle bottom + x
    _vertices.push_back(Vector4(1.0, 1.0, 1.0, 1.0));
    _vertices.push_back(Vector4(1.0, 1.0, -1.0, 1.0));
    _vertices.push_back(Vector4(1.0, -1.0, -1.0, 1.0));

    //zy plane triangle top - x
    _vertices.push_back(Vector4(-1.0, 1.0, 1.0, 1.0));
    _vertices.push_back(Vector4(-1.0, 1.0, -1.0, 1.0));
    _vertices.push_back(Vector4(-1.0, -1.0, -1.0, 1.0));

    //zy plane triangle bottom - x
    _vertices.push_back(Vector4(-1.0, 1.0, 1.0, 1.0));
    _vertices.push_back(Vector4(-1.0, -1.0, 1.0, 1.0));
    _vertices.push_back(Vector4(-1.0, -1.0, -1.0, 1.0));

    //Now flatten it out for opengl
    size_t triBuffSize = _vertices.size() * 3;
    float* flattenVerts = new float[triBuffSize]; //Only include the x y and z values not w
    int i = 0; //iterates through vertices indexes
    for (auto vertex : _vertices) {
        float *flat = vertex.getFlatBuffer();
        flattenVerts[i++] = flat[0];
        flattenVerts[i++] = flat[1];
        flattenVerts[i++] = flat[2];
    }

    //Create a buffer that will be filled the vertex data
    glGenBuffers(1, &_bufferContext);
    glBindBuffer(GL_ARRAY_BUFFER, _bufferContext);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float)*triBuffSize, flattenVerts, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

}

void Model::updateDraw() {

    //LOAD IN SHADER
    glUseProgram(_shaderProgram.getShaderContext()); //use context for loaded shader

    //LOAD IN BUFFER 
    //Bind this context to current buffer
    glBindBuffer(GL_ARRAY_BUFFER, _bufferContext);

    //Say that the vertex data is associated with attribute 0 in the context of a shader program
    //Each vertex contains 3 floats per vertex
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

    //Now enable it at location 0
    glEnableVertexAttribArray(0);

	//glUniform mat4 combined model and world matrix
	glUniformMatrix4fv(_shaderProgram.getModelLocation(), 1, GL_FALSE, _model.getGLFormat().getFlatBuffer());
	
	//glUniform mat4 view matrix
	glUniformMatrix4fv(_shaderProgram.getViewLocation(), 1, GL_FALSE, _view.getGLFormat().getFlatBuffer());
	
	//glUniform mat4 projection matrix
	glUniformMatrix4fv(_shaderProgram.getProjectionLocation(), 1, GL_FALSE, _projection.getGLFormat().getFlatBuffer());
	
    //Draw triangles using the bound buffer vertices at starting index 0 and number of triangles
    glDrawArraysEXT(GL_TRIANGLES, 0, (GLsizei)_vertices.size());

	glDisableVertexAttribArray (0); //Disable attribute
	glBindBuffer(GL_ARRAY_BUFFER, 0); //Unbind buffer
    glUseProgram(0);//end using this shader
}

void Model::updateKeyboard(unsigned char key, int x, int y){
	//std::cout << key << std::endl;
}
	
void Model::updateMouse(int button, int state, int x, int y){
	//std::cout << x << " " << y << std::endl;
}

void Model::updateView(Matrix view){
	_view = view; //Receive updates when the view matrix has changed
	//_view.display();
}

void Model::updateProjection(Matrix projection){
	_projection = projection; //Receive updates when the projection matrix has changed
	//_projection.display();
}
