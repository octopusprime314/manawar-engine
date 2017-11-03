#include "Model.h"
#include "SimpleContext.h"
#include "FbxLoader.h"
#include "GeometryBuilder.h"

Model::Model(){

}

Model::Model(std::string name, ViewManagerEvents* eventWrapper, ModelClass classId) : _fbxLoader(nullptr), UpdateInterface(eventWrapper){

    //Set class id
    _classId = classId;

	//disable debug mode
	_debugMode = false;

	//Load default shader
	_shaderProgram = new Shader();
	_shaderProgram->build();

	//Load debug shader
	_debugShaderProgram = new DebugShader();
	_debugShaderProgram->build();

    //Load in fbx object 
    _fbxLoader = new FbxLoader(name);
    //Populate model with fbx file data and recursivelty search with the root node of the scene
    _fbxLoader->loadModel(this, _fbxLoader->getScene()->GetRootNode());

    //If class is generic model then deallocate fbx object,
    //otherwise let derived class clean up _fbxLoader object
    //because the derived class may need to access data from it still
    if (_classId == ModelClass::ModelType) {
        delete _fbxLoader;
    }

    GeometryBuilder::buildCube(this); //Add a generic cube centered at the origin

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

Model::~Model() {
    delete _fbxLoader;
    delete _debugShaderProgram;
    delete _shaderProgram;
}

void Model::updateDraw() {

	//if debugging normals, etc.
	if(_debugMode){

		//Run debug model shader by allowing the shader to operate on the model
		_debugShaderProgram->runShader(this);
	}

	//Run model shader by allowing the shader to operate on the model
	_shaderProgram->runShader(this);

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
}

void Model::updateProjection(Matrix projection){
	_projection = projection; //Receive updates when the projection matrix has changed
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

std::vector<Vector4> Model::getVertices() {
    return _vertices;
}

std::vector<Vector4> Model::getNormals() {
    return _normals;
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