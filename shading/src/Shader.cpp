#include "Shader.h"
#include "Model.h"
#include "Light.h"

Shader::Shader(std::string shaderName) {
	//set name
	_shaderName = shaderName;
	//build it
	_build();
}

Shader::~Shader() {

}

void Shader::_build() {
    unsigned int vertexShaderHandle;
    unsigned int fragmentShaderHandle;

    std::string fileNameVert = SHADERS_LOCATION + _shaderName;
    fileNameVert.append(".vert");
    std::string fileNameFrag = SHADERS_LOCATION + _shaderName;
    fileNameFrag.append(".frag");

    //Compile each shader
    vertexShaderHandle = _compile((char*)fileNameVert.c_str(), GL_VERTEX_SHADER);
    fragmentShaderHandle = _compile((char*)fileNameFrag.c_str(), GL_FRAGMENT_SHADER);

    //Link the two compiled binaries
    _link(vertexShaderHandle, fragmentShaderHandle);
}

void Shader::_link(unsigned int vertexShaderHandle, unsigned int fragmentShaderHandle) {
    _shaderContext = glCreateProgram();

    glAttachShader(_shaderContext, vertexShaderHandle);
    glAttachShader(_shaderContext, fragmentShaderHandle);

    glLinkProgram(_shaderContext);

    GLint      successfully_linked = 0;
    glGetProgramiv(_shaderContext, GL_LINK_STATUS, &successfully_linked);

    // Exit if the program couldn't be linked correctly
    if (!successfully_linked) {
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
    if (err != 0)
    {
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
    if (!result) {
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
