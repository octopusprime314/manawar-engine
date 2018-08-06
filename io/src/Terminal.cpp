#include "Terminal.h"
#include "SimpleContextEvents.h"
#include "Picker.h"

ShaderBroker* Terminal::_shaderManager = ShaderBroker::instance();
ModelBroker*  Terminal::_modelManager  = ModelBroker::instance();

using namespace std::placeholders;

Terminal::Terminal(MRTFrameBuffer* gBuffers, std::vector<Entity*> entityList) :
    _fontRenderer("ubuntu_mono_regular.fnt"),
    _gameState(0),
    _commandString("|"),
    _commandHistoryIndex(0) {

    _picker = new Picker(gBuffers, std::bind(&Terminal::_mousePosition, this, _1));
    _picker->addPickableEntities(entityList);

    SimpleContextEvents::subscribeToKeyboard(std::bind(&Terminal::_updateKeyboard, this, _1, _2, _3));
    SimpleContextEvents::subscribeToGameState(std::bind(&Terminal::_updateGameState, this, _1));
}

Terminal::~Terminal() {

}

void Terminal::display() {
    
    if (_gameState == 1) {

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
            _modelManager->addModel(modelName, modelToAdd, Vector4(
                static_cast<float>(atof(xStr.c_str())), 
                static_cast<float>(atof(yStr.c_str())),
                static_cast<float>(atof(zStr.c_str())), 
                static_cast<float>(atof(scale.c_str()))));
        }
        else if (command == "MOUSEADD") {
            _commandToProcess = _commandToProcess.substr(_commandToProcess.find(' ') + 1);
            std::string modelName = _commandToProcess.substr(0, _commandToProcess.find(' '));
            _commandToProcess = _commandToProcess.substr(_commandToProcess.find(' ') + 1);
            std::string modelToAdd = _commandToProcess.substr(0, _commandToProcess.find(' '));
            modelToAdd.pop_back();
            /*_modelManager->addModel(modelName, modelToAdd, Vector4(
                static_cast<float>(atof(xStr.c_str())),
                static_cast<float>(atof(yStr.c_str())),
                static_cast<float>(atof(zStr.c_str())),
                static_cast<float>(atof(scale.c_str()))));*/
        }
        else if (command == "SAVE") {
            _commandToProcess = _commandToProcess.substr(_commandToProcess.find(' ') + 1);
            std::string modelName = _commandToProcess.substr(0, _commandToProcess.find(' '));
            modelName.pop_back();
            _modelManager->saveModel(modelName);
        }
        else if (command == "CLEAR") {
            _commandToProcess = _commandToProcess.substr(_commandToProcess.find(' ') + 1);
            std::string modelName = _commandToProcess.substr(0, _commandToProcess.find(' '));
            modelName.pop_back();
            _modelManager->clearChanges(modelName);
        }
        else if (command == "UNDO") {
            //TODO: Undo previous change to scene
        }
        _commandToProcess = "";
    }
}

void Terminal::_mousePosition(Vector4 position) {

    if (_commandHistory.size() > 0) {

        auto commandString = _commandHistory[_commandHistoryIndex];
        auto location = commandString.find(' ');
        std::string command = commandString.substr(0, location);

        if (command == "MOUSEADD") {
            commandString = commandString.substr(commandString.find(' ') + 1);
            std::string modelName = commandString.substr(0, commandString.find(' '));
            commandString = commandString.substr(commandString.find(' ') + 1);
            std::string modelToAdd = commandString.substr(0, commandString.find(' '));
            modelToAdd.pop_back();

            _modelManager->addModel(modelName, modelToAdd, position);
        }
    }
}

void Terminal::_updateKeyboard(int key, int x, int y) { //Do stuff based on keyboard update

    //Process command if entered correctly
    if (_gameState == 1 && key == GLFW_KEY_ENTER) {
        _commandToProcess = _commandString;
        _commandHistory.push_back(_commandToProcess);
    }

    if (_gameState == 1 &&
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
        else if (key == GLFW_KEY_UP) {
            if (_commandHistoryIndex > 0) {
                _commandHistoryIndex--;
                _commandString = _commandHistory[_commandHistoryIndex];
            }
        }
        else if (key == GLFW_KEY_DOWN) {
            if (_commandHistoryIndex < _commandHistory.size() - 1) {
                _commandHistoryIndex++;
                _commandString = _commandHistory[_commandHistoryIndex];
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
}

void Terminal::_updateGameState(int state) {
    _gameState = state;
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