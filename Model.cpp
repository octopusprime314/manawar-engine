#include "Model.h"
#include "SimpleContext.h"
#include "FbxLoader.h"

Model::Model(){

}

Model::Model(ViewManagerEvents* eventWrapper) : UpdateInterface(eventWrapper){

	//disable debug mode
	_debugMode = false;

	//Load default shader
	_shaderProgram = new Shader();
	_shaderProgram->build();

	//Load debug shader
	_debugShaderProgram = new DebugShader();
	_debugShaderProgram->build();

    //Load in fbx object
    FbxLoader fbxLoad("../models/landscape.fbx");
	//Populate model with fbx file data and recursivelty search with the root node of the scene
    fbxLoad.loadModel(this, fbxLoad.getScene()->GetRootNode()); 

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

	//Normal per vertex ratio

	//xy plane triangle top + z
    _normals.push_back(Vector4(0.0, 0.0, 1.0, 1.0));
    _normals.push_back(Vector4(0.0, 0.0, 1.0, 1.0));
    _normals.push_back(Vector4(0.0, 0.0, 1.0, 1.0));

    //xy plane triangle bottom + z
    _normals.push_back(Vector4(0.0, 0.0, 1.0, 1.0));
    _normals.push_back(Vector4(0.0, 0.0, 1.0, 1.0));
    _normals.push_back(Vector4(0.0, 0.0, 1.0, 1.0));

    //xy plane triangle top - z
    _normals.push_back(Vector4(0.0, 0.0, -1.0, 1.0));
    _normals.push_back(Vector4(0.0, 0.0, -1.0, 1.0));
	_normals.push_back(Vector4(0.0, 0.0, -1.0, 1.0));

    //xy plane triangle bottom - z
    _normals.push_back(Vector4(0.0, 0.0, -1.0, 1.0));
    _normals.push_back(Vector4(0.0, 0.0, -1.0, 1.0));
	_normals.push_back(Vector4(0.0, 0.0, -1.0, 1.0));

    //zy plane triangle top + x
    _normals.push_back(Vector4(1.0, 0.0, 0.0, 1.0));
    _normals.push_back(Vector4(1.0, 0.0, 0.0, 1.0));
    _normals.push_back(Vector4(1.0, 0.0, 0.0, 1.0));

    //zy plane triangle bottom + x
    _normals.push_back(Vector4(1.0, 0.0, 0.0, 1.0));
    _normals.push_back(Vector4(1.0, 0.0, 0.0, 1.0));
    _normals.push_back(Vector4(1.0, 0.0, 0.0, 1.0));

    //zy plane triangle top - x
    _normals.push_back(Vector4(-1.0, 0.0, 0.0, 1.0));
    _normals.push_back(Vector4(-1.0, 0.0, 0.0, 1.0));
    _normals.push_back(Vector4(-1.0, 0.0, 0.0, 1.0));

    //zy plane triangle bottom - x
    _normals.push_back(Vector4(-1.0, 0.0, 0.0, 1.0));
    _normals.push_back(Vector4(-1.0, 0.0, 0.0, 1.0));
    _normals.push_back(Vector4(-1.0, 0.0, 0.0, 1.0));


	//Lines for normal visualization

	//xy plane triangle top + z
    _normalLineVertices.push_back(_vertices[0]);
	_normalLineVertices.push_back(_vertices[0] + _normals[0]);
    _normalLineVertices.push_back(_vertices[1]);
	_normalLineVertices.push_back(_vertices[1] + _normals[1]);
	_normalLineVertices.push_back(_vertices[2]);
	_normalLineVertices.push_back(_vertices[2] + _normals[2]);

	//xy plane triangle bottom + z
	_normalLineVertices.push_back(_vertices[3]);
	_normalLineVertices.push_back(_vertices[3] + _normals[3]);
    _normalLineVertices.push_back(_vertices[4]);
	_normalLineVertices.push_back(_vertices[4] + _normals[4]);
	_normalLineVertices.push_back(_vertices[5]);
	_normalLineVertices.push_back(_vertices[5] + _normals[5]);

	//xy plane triangle top - z
	_normalLineVertices.push_back(_vertices[6]);
	_normalLineVertices.push_back(_vertices[6] + _normals[6]);
    _normalLineVertices.push_back(_vertices[7]);
	_normalLineVertices.push_back(_vertices[7] + _normals[7]);
	_normalLineVertices.push_back(_vertices[8]);
	_normalLineVertices.push_back(_vertices[8] + _normals[8]);

	//xy plane triangle bottom - z
	_normalLineVertices.push_back(_vertices[9]);
	_normalLineVertices.push_back(_vertices[9] + _normals[9]);
    _normalLineVertices.push_back(_vertices[10]);
	_normalLineVertices.push_back(_vertices[10] + _normals[10]);
	_normalLineVertices.push_back(_vertices[11]);
	_normalLineVertices.push_back(_vertices[11] + _normals[11]);

    //zy plane triangle top + x
    _normalLineVertices.push_back(_vertices[12]);
	_normalLineVertices.push_back(_vertices[12] + _normals[12]);
    _normalLineVertices.push_back(_vertices[13]);
	_normalLineVertices.push_back(_vertices[13] + _normals[13]);
	_normalLineVertices.push_back(_vertices[14]);
	_normalLineVertices.push_back(_vertices[14] + _normals[14]);

    //zy plane triangle bottom + x
    _normalLineVertices.push_back(_vertices[15]);
	_normalLineVertices.push_back(_vertices[15] + _normals[15]);
    _normalLineVertices.push_back(_vertices[16]);
	_normalLineVertices.push_back(_vertices[16] + _normals[16]);
	_normalLineVertices.push_back(_vertices[17]);
	_normalLineVertices.push_back(_vertices[17] + _normals[17]);

    //zy plane triangle top - x
    _normalLineVertices.push_back(_vertices[18]);
	_normalLineVertices.push_back(_vertices[18] + _normals[18]);
    _normalLineVertices.push_back(_vertices[19]);
	_normalLineVertices.push_back(_vertices[19] + _normals[19]);
	_normalLineVertices.push_back(_vertices[20]);
	_normalLineVertices.push_back(_vertices[20] + _normals[20]);

    //zy plane triangle bottom - x
    _normalLineVertices.push_back(_vertices[21]);
	_normalLineVertices.push_back(_vertices[21] + _normals[21]);
    _normalLineVertices.push_back(_vertices[22]);
	_normalLineVertices.push_back(_vertices[22] + _normals[22]);
	_normalLineVertices.push_back(_vertices[23]);
	_normalLineVertices.push_back(_vertices[23] + _normals[23]);


    //Now flatten vertices and normals out for opengl
    size_t triBuffSize = _vertices.size() * 3;
    float* flattenVerts = new float[triBuffSize]; //Only include the x y and z values not w
	float* flattenNorms = new float[triBuffSize]; //Only include the x y and z values not w, same size as vertices
	size_t lineBuffSize = _normalLineVertices.size() * 3;
	float* flattenNormLines = new float[lineBuffSize]; //Only include the x y and z values not w, flat line data
    int i = 0; //iterates through vertices indexes
    for (auto vertex : _vertices) {
        float *flat = vertex.getFlatBuffer();
        flattenVerts[i++] = flat[0];
        flattenVerts[i++] = flat[1];
        flattenVerts[i++] = flat[2];
    }
	i = 0; //Reset for normal indexes
	for (auto normal : _normals) {
        float *flat = normal.getFlatBuffer();
        flattenNorms[i++] = flat[0];
        flattenNorms[i++] = flat[1];
        flattenNorms[i++] = flat[2];
    }

	i = 0; //Reset for normal line indexes
	for (auto normalLine : _normalLineVertices) {
        float *flat = normalLine.getFlatBuffer();
        flattenNormLines[i++] = flat[0];
        flattenNormLines[i++] = flat[1];
        flattenNormLines[i++] = flat[2];
    }

    //Create a buffer that will be filled with the vertex data
    glGenBuffers(1, &_vertexBufferContext);
    glBindBuffer(GL_ARRAY_BUFFER, _vertexBufferContext);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float)*triBuffSize, flattenVerts, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

	//Create a buffer that will be filled with the normal data
    glGenBuffers(1, &_normalBufferContext);
    glBindBuffer(GL_ARRAY_BUFFER, _normalBufferContext);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float)*triBuffSize, flattenNorms, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

	//Create a buffer that will be filled with the normal line data for visualizing normals
    glGenBuffers(1, &_debugNormalBufferContext);
    glBindBuffer(GL_ARRAY_BUFFER, _debugNormalBufferContext);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float)*lineBuffSize, flattenNormLines, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
	
}

void Model::updateDraw() {

	//if debugging normals, etc.
	if(_debugMode){

		//Run debug model shader by allowing the shader to operate on the model
		_debugShaderProgram->runShader(this);
	}

	//Run model shader by allowing the shader to operate on the model
	_shaderProgram->runShader(this);

	Vector4 transformedNormal = _normal * _normals[0];
	transformedNormal.display();
}

void Model::updateKeyboard(unsigned char key, int x, int y){
	//std::cout << key << std::endl;
}
	
void Model::updateMouse(int button, int state, int x, int y){
	//std::cout << x << " " << y << std::endl;
}

void Model::updateView(Matrix view){
	_view = view; //Receive updates when the view matrix has changed
	
	//If view changes then change our normal matrix
	_normal = _view.inverse().transpose();

	_normal.display();
}

void Model::updateProjection(Matrix projection){
	_projection = projection; //Receive updates when the projection matrix has changed
	//_projection.display();
}

Matrix Model::getModel(){
	return _model;
}

Matrix Model::getView(){
	return _view;
}

Matrix Model::getProjection(){
	return _projection;
}

Matrix Model::getNormal(){
	return _normal;
}

float* Model::getModelBuffer(){
	return _model.getFlatBuffer();
}

float* Model::getViewBuffer(){
	return _view.getFlatBuffer();
}

float* Model::getProjectionBuffer(){
	return _projection.getFlatBuffer();
}

float* Model::getNormalBuffer(){
	return _normal.getFlatBuffer();
}

GLuint Model::getVertexContext(){
	return _vertexBufferContext;
}
	
GLuint Model::getNormalContext(){
	return _normalBufferContext;
}

GLuint Model::getNormalDebugContext(){
	return _debugNormalBufferContext;
}

size_t Model::getVertexCount(){
	return _vertices.size();
}

size_t Model::getNormalLineCount(){
	return _normalLineVertices.size();
}

void Model::addVertex(Vector4 vertex) {
    _vertices.push_back(vertex);
}

void Model::addNormal(Vector4 normal) {
    _normals.push_back(normal);
}

void Model::addDebugNormal(Vector4 normal){
	_normalLineVertices.push_back(normal);
}