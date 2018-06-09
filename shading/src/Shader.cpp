#include "Shader.h"
#include "Model.h"
#include "Light.h"
#include <iostream>
#include <fstream>

// You can hit this in a debugger.
// Set to 'true' to printf every shader that is linked or compiled.
static volatile bool g_VerboseShaders = true;

Shader::Shader(std::string vertexShaderName, std::string fragmentShaderName) {
	//set vertex name
	_vertexShaderName = vertexShaderName;
    //set fragment name if different from vertex shader name
    if (fragmentShaderName != "") {
        _fragmentShaderName = fragmentShaderName;
    }
	//build it
	_build();
}

Shader::~Shader() {

}

inline bool fileExists(const std::string& name) {
    std::ifstream f(name.c_str());
    return f.good();
}

void Shader::_build() {
    unsigned int vertexSH;
    unsigned int fragmentSH;
    unsigned int geomSH;
    unsigned int computeSH;

    std::string fileNameVert = SHADERS_LOCATION + _vertexShaderName;
    fileNameVert.append(".vert");
    std::string fileNameFrag = SHADERS_LOCATION;
    if (_fragmentShaderName != "") {
        fileNameFrag += _fragmentShaderName;
    }
    else {
        fileNameFrag += _vertexShaderName;
    }
    fileNameFrag.append(".frag");
    std::string fileNameGeom = SHADERS_LOCATION + _vertexShaderName;
    fileNameGeom.append(".geom");
    std::string fileNameCompute = SHADERS_LOCATION + _vertexShaderName;
    fileNameCompute.append(".comp");

    //Compile each shader
    if (fileExists(fileNameVert)) {
        vertexSH = _compile((char*)fileNameVert.c_str(), GL_VERTEX_SHADER);
    }
    else {
        vertexSH = -1;
    }
    if (fileExists(fileNameFrag)) {
        fragmentSH = _compile((char*)fileNameFrag.c_str(), GL_FRAGMENT_SHADER);
    }
    else {
        fragmentSH = -1;
    }
    if (fileExists(fileNameGeom)) {
        geomSH = _compile((char*)fileNameGeom.c_str(), GL_GEOMETRY_SHADER);
    }
    else {
        geomSH = -1;
    }
    if (fileExists(fileNameCompute)) {
        computeSH = _compile((char*)fileNameCompute.c_str(), GL_COMPUTE_SHADER);
    }
    else {
        computeSH = -1;
    }

    //Link shaders
    _link(vertexSH, fragmentSH, geomSH, computeSH);
}

void Shader::_link(unsigned int vertexSH, unsigned int fragmentSH, unsigned int geomSH, unsigned int computeSH) {
    _shaderContext = glCreateProgram();

    if (vertexSH != -1) {
        glAttachShader(_shaderContext, vertexSH);
    }
    if (fragmentSH != -1) {
        glAttachShader(_shaderContext, fragmentSH);
    }
    if (geomSH != -1) {
        glAttachShader(_shaderContext, geomSH);
    }
    if (computeSH != -1) {
        glAttachShader(_shaderContext, computeSH);
    }

    glLinkProgram(_shaderContext);

    GLint      successfully_linked = 0;
    glGetProgramiv(_shaderContext, GL_LINK_STATUS, &successfully_linked);

    // Exit if the program couldn't be linked correctly
    if (successfully_linked) {
        if (g_VerboseShaders) {
            printf("Shader #%d linked   (Shader* this = %p)\n", _shaderContext, this);
        }
    } else {
        GLint errorLoglength;
        GLint actualErrorLogLength;
        //Attempt to get the length of our error log.
        glGetProgramiv(_shaderContext, GL_INFO_LOG_LENGTH, &errorLoglength);

        std::cout << errorLoglength << std::endl;

        //Create a buffer to read compilation error message
        char* errorLogText = (char*)malloc(sizeof(char) * errorLoglength);

        //Used to get the final length of the log.
        glGetProgramInfoLog(_shaderContext, errorLoglength, &actualErrorLogLength, errorLogText);

        std::cout << actualErrorLogLength << std::endl;

        // Display errors.
        std::cout << errorLogText << std::endl;

        // Free the buffer malloced earlier
        free(errorLogText);

        std::cout << "Program was not linked correctly!" << std::endl;
    }
}

// Loading shader function
unsigned int Shader::_compile(char* filename, unsigned int type)
{
    FILE *pfile;
    unsigned int handle;
    const GLchar* files[1];

    // shader Compilation variable
    GLint result;				// Compilation code result
    GLint errorLoglength;
    char* errorLogText;
    GLsizei actualErrorLogLength;

    char buffer[400000];
    memset(buffer, 0, 400000);

    errno_t err = fopen_s(&pfile, filename, "rb");
    if (err != 0) {
        printf("Sorry, can't open file: '%s'.\n", filename);
        return 0;
    }

    fread(buffer, sizeof(char), 400000, pfile);

    fclose(pfile);

    handle = glCreateShader(type);
    if (!handle) {
        //We have failed creating the vertex shader object.
        printf("Failed creating vertex shader object from file: %s.", filename);
        return 0;
    }

    files[0] = (const GLchar*)buffer;
    glShaderSource(
        handle, //The handle to our shader
        1, //The number of files.
        files, //An array of const char * data, which represents the source code of theshaders
        nullptr);

     glCompileShader(handle);

    //Compilation checking.
    glGetShaderiv(handle, GL_COMPILE_STATUS, &result);

    // If an error was detected.
    if (result) {
        if (g_VerboseShaders) {
            printf("Shader #%d compiled (filename = \"%s\")\n", handle, filename);
        }
    } else {
        //We failed to compile.
        printf("Shader '%s' failed compilation.\n", filename);

        //Attempt to get the length of our error log.
        glGetShaderiv(handle, GL_INFO_LOG_LENGTH, &errorLoglength);

        //Create a buffer to read compilation error message
        errorLogText = (char*)malloc(sizeof(char) * errorLoglength);

        //Used to get the final length of the log.
        glGetShaderInfoLog(handle, errorLoglength, &actualErrorLogLength, errorLogText);

        // Display errors.
        printf("%s\n", errorLogText);

        // Free the buffer malloced earlier
        free(errorLogText);
    }

    return handle;
}

GLint Shader::getShaderContext() {
    return _shaderContext;
}
