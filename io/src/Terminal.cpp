#include "Terminal.h"
#include "SimpleContextEvents.h"

ShaderBroker* Terminal::_shaderManager = ShaderBroker::instance();
ModelBroker*  Terminal::_modelManager  = ModelBroker::instance();

using namespace std::placeholders;

Terminal::Terminal() :
    _fontRenderer("ubuntu_mono_regular.fnt"),
    _terminalVisible(false),
    _commandString("|") {

    SimpleContextEvents::subscribeToKeyboard(std::bind(&Terminal::_updateKeyboard, this, _1, _2, _3));
}

Terminal::~Terminal() {

}

void Terminal::display() {
    
    if (_terminalVisible) {
        glClear(GL_DEPTH_BUFFER_BIT);
        _fontRenderer.drawFont(0.0f, -1.9f, _commandString, 0);
    }

    if (_commandToProcess.size() > 0) {

        auto location = _commandToProcess.find(' ');
        std::string command = _commandToProcess.substr(0, location);

        if (command == "RECOMPILE") {
            std::string shaderName = _commandToProcess.substr(location + 1);
            shaderName.pop_back();
            _shaderManager->recompileShader(shaderName);
        }
        else if (command == "UPDATE") {
            std::string modelName = _commandToProcess.substr(location + 1);
            modelName.pop_back();
            _modelManager->updateModel(modelName);
        }
        else if (command == "ADD") {
            _commandToProcess = _commandToProcess.substr(_commandToProcess.find(' ') + 1);
            std::string modelName = _commandToProcess.substr(0, _commandToProcess.find(' '));
            _commandToProcess = _commandToProcess.substr(_commandToProcess.find(' ') + 1);
            std::string modelToAdd = _commandToProcess.substr(0, _commandToProcess.find(' '));
            _commandToProcess = _commandToProcess.substr(_commandToProcess.find(' ') + 1);
            std::string xStr = _commandToProcess.substr(0, _commandToProcess.find(' '));
            _commandToProcess = _commandToProcess.substr(_commandToProcess.find(' ') + 1);
            std::string yStr = _commandToProcess.substr(0, _commandToProcess.find(' '));
            _commandToProcess = _commandToProcess.substr(_commandToProcess.find(' ') + 1);
            std::string zStr = _commandToProcess.substr(0, _commandToProcess.find(' '));
            _commandToProcess = _commandToProcess.substr(_commandToProcess.find(' ') + 1);
            std::string scale = _commandToProcess.substr(0, _commandToProcess.find(' '));
            scale.pop_back();
            _modelManager->addModel(modelName, modelToAdd, Vector4(atof(xStr.c_str()), atof(yStr.c_str()), atof(zStr.c_str()), atof(scale.c_str())));
        }
        else if (command == "SAVE") {
            _commandToProcess = _commandToProcess.substr(_commandToProcess.find(' ') + 1);
            std::string modelName = _commandToProcess.substr(0, _commandToProcess.find(' '));
            modelName.pop_back();
            _modelManager->saveModel(modelName);
        }
        _commandToProcess = "";
    }
}

bool Terminal::inTerminalMode() {
    return _terminalVisible;
}

void Terminal::_updateKeyboard(int key, int x, int y) { //Do stuff based on keyboard update

    //Process command if entered correctly
    if (_terminalVisible && key == GLFW_KEY_ENTER) {
        _commandToProcess = _commandString;
    }

    if (_terminalVisible && 
        key != GLFW_KEY_GRAVE_ACCENT &&
        key != GLFW_KEY_ENTER) {

        if (key == GLFW_KEY_LEFT || key == GLFW_KEY_RIGHT) {

            int i = _getCursorIndex();
            if ((i > 0 && key == GLFW_KEY_LEFT) ||
                (i < _commandString.size() - 1 && key == GLFW_KEY_RIGHT)) {
                if (key == GLFW_KEY_LEFT) {
                    auto temp = _commandString[i - 1];
                    _commandString[i - 1] = '|';
                    _commandString[i] = temp;
                }
                else {
                    auto temp = _commandString[i + 1];
                    _commandString[i + 1] = '|';
                    _commandString[i] = temp;
                }
            }
        }
        else if (key == GLFW_KEY_BACKSPACE) {

            int i = _getCursorIndex();
            if (i > 0) {
                _commandString = _commandString.substr(0, i - 1) + '|' + _commandString.substr(i + 1);
            }
        }
        else {
            int i = _getCursorIndex();
            if (i != _commandString.size() - 1 && _commandString.size() > 1) {
                _commandString = _commandString.substr(0, i) + char(key) + '|' + _commandString.substr(i + 1);
            }
            else {
                _commandString[i] = char(key);
                _commandString += '|';
            }
        }
    }

    //Grave accent is the ~ key
    if (key == GLFW_KEY_GRAVE_ACCENT) {
        _terminalVisible = !_terminalVisible;
    }
}

int Terminal::_getCursorIndex() {

    int i = 0;
    for (auto& a : _commandString) {
        if (a == '|') {
            return i;
        }
        i++;
    }
    return -1;
}