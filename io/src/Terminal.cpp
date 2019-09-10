#include "Terminal.h"
#include "IOEvents.h"
#include "Picker.h"
#include <algorithm>
#include "EngineManager.h"
#include <iostream>
#include <string>

ShaderBroker* Terminal::_shaderManager    = ShaderBroker::instance();
ModelBroker*  Terminal::_modelManager     = ModelBroker::instance();
Terminal*     Terminal::_terminalInstance = nullptr;


using namespace std::placeholders;

void Terminal::initTerminal(MRTFrameBuffer*      gBuffers,
                            std::vector<Entity*> entityList) {

     _gBuffers = gBuffers;

    _picker    = new Picker(gBuffers,
                            std::bind(&Terminal::_mousePosition, this, _1, _2),
                            std::bind(&Terminal::_mouseDeletion, this, _1));
}

Terminal::Terminal() :
    _fontRenderer("ubuntu_mono_regular.fnt"),
    _gameState(EngineState::getEngineState()),
    _commandString("|"),
    _commandHistoryIndex(0),
    _gBuffers(nullptr),
    _modelNameIndex(0) {

        // Added global history for quick debugging of model creator
    _commandHistory.push_back("ADDTILE SANDBOX TERRAINTILE 0 0 0 1 SNOW.JPG DIRT.JPG ROCKS.JPG GRASS.JPG|");
    _commandHistory.push_back("MOUSEADD SANDBOX DEADTREE 0.0 0.0 0.0 0.025|");
    _commandHistory.push_back("MOUSEDELETE SANDBOX|");
    _commandHistory.push_back("SAVE SANDBOX|");

    IOEvents::subscribeToKeyboard(std::bind(&Terminal::_updateKeyboard, this, _1, _2, _3));
    IOEvents::subscribeToReleaseKeyboard(std::bind(&Terminal::_updateReleaseKeyboard, this, _1, _2, _3));
    IOEvents::subscribeToGameState(std::bind(&Terminal::_updateGameState, this, _1));
}

Terminal::~Terminal() { 
    delete _terminalInstance;
}

void Terminal::processCommand(std::string command) {
    // If mouse pathing mode is enabled then edit tiles with terrain painting system
    if (command.find("MOUSEPATHING") != std::string::npos) {

        command = command.substr(command.find_first_of(" ") + 1);
        int button       = std::atoi(command.substr(0, command.find_first_of(" ")).c_str());
        command          = command.substr(command.find_first_of(" ") + 1);
        int action       = std::atoi(command.substr(0, command.find_first_of(" ")).c_str());
        command          = command.substr(command.find_first_of(" ") + 1);
        int tileWidthX   = std::atoi(command.substr(0, command.find_first_of(" ")).c_str());
        command          = command.substr(command.find_first_of(" ") + 1);
        int tileWidthY   = std::atoi(command.substr(0, command.find_first_of(" ")).c_str());
        command          = command.substr(command.find_first_of(" ") + 1);
        int entityID     = std::atoi(command.substr(0, command.find_first_of(" ")).c_str());
        command          = command.substr(command.find_first_of(" ") + 1);

        // Button is left or middle button mouse click.  0 is left, 2 is middle.
        // Left indicates a write to terrain and Middle indicates to switch to the next texture option.
        // Action is press or release mouse button.  0 is release, 1 is press.
        // X and Y are the screen space location of the mouse button click or release.
        _picker->editTile(button, action, tileWidthX, tileWidthY, entityID);
    } else {
        _commandProcessor(command);
    }
}

Terminal* Terminal::instance() {
    if (_terminalInstance == nullptr) {
        _terminalInstance = new Terminal();
    }
    return _terminalInstance;
}

void Terminal::_commandProcessor(std::string command) {
    auto        location    = command.find(' ');
    std::string commandType = command.substr(0, location);

    if (commandType == "RECOMPILE") {
        std::string shaderName = command.substr(command.find(' ') + 1);
        _shaderManager->recompileShader(shaderName);
    }
    else if (commandType == "UPDATE") {
        std::string modelName = command.substr(command.find(' ') + 1);
        _modelManager->updateModel(modelName);
    }
    else if (commandType == "ADD") {
        command                = command.substr(command.find(' ') + 1);
        std::string modelName  = command.substr(0, command.find(' '));
        command                = command.substr(command.find(' ') + 1);
        std::string modelToAdd = command.substr(0, command.find(' '));
        command                = command.substr(command.find(' ') + 1);
        std::string xStr       = command.substr(0, command.find(' '));
        command                = command.substr(command.find(' ') + 1);
        std::string yStr       = command.substr(0, command.find(' '));
        command                = command.substr(command.find(' ') + 1);
        std::string zStr       = command.substr(0, command.find(' '));
        command                = command.substr(command.find(' ') + 1);
        std::string scale      = command.substr(0, command.find(' '));

        _modelManager->addModel(modelName,
                                modelToAdd,
                                Vector4(static_cast<float>(atof(xStr.c_str())),
                                        static_cast<float>(atof(yStr.c_str())),
                                        static_cast<float>(atof(zStr.c_str())),
                                        static_cast<float>(atof(scale.c_str()))),
                                Vector4(0.0, 0.0, 0.0));
    }
    else if (commandType == "ADDTILE") {
        command                = command.substr(command.find(' ') + 1);
        std::string modelName  = command.substr(0, command.find(' '));
        command                = command.substr(command.find(' ') + 1);
        std::string modelToAdd = command.substr(0, command.find(' '));
        command                = command.substr(command.find(' ') + 1);
        std::string xStr       = command.substr(0, command.find(' '));
        command                = command.substr(command.find(' ') + 1);
        std::string yStr       = command.substr(0, command.find(' '));
        command                = command.substr(command.find(' ') + 1);
        std::string zStr       = command.substr(0, command.find(' '));
        command                = command.substr(command.find(' ') + 1);
        std::string scale      = command.substr(0, command.find(' '));
        command                = command.substr(command.find(' ') + 1);
        std::string rChannel   = command.substr(0, command.find(' '));
        command                = command.substr(command.find(' ') + 1);
        std::string gChannel   = command.substr(0, command.find(' '));
        command                = command.substr(command.find(' ') + 1);
        std::string bChannel   = command.substr(0, command.find(' '));
        command                = command.substr(command.find(' ') + 1);
        std::string aChannel   = command.substr(0, command.find(' '));
        _modelManager->addTileModel(modelName,
                                    modelToAdd,
                                    Vector4(static_cast<float>(atof(xStr.c_str())),
                                            static_cast<float>(atof(yStr.c_str())),
                                            static_cast<float>(atof(zStr.c_str())),
                                            static_cast<float>(atof(scale.c_str()))), 
                                    { rChannel, gChannel, bChannel, aChannel });
    }
    else if (commandType == "MOUSEADD") {
        command                = command.substr(command.find(' ') + 1);
        std::string modelName  = command.substr(0, command.find(' '));
        command                = command.substr(command.find(' ') + 1);
        std::string modelToAdd = command.substr(0, command.find(' '));

        //Updates currently selected model that will be placed on entities
        _mousePosition(_picker->getLastPickedPosition(), false);

        if (modelToAdd == "*") {
            auto modelCount = _modelManager->getModelNames().size();
            _modelNameIndex++;
            _modelNameIndex = _modelNameIndex % modelCount;
        }
    }
    else if (commandType == "SAVE") {
        command               = command.substr(command.find(' ') + 1);
        std::string modelName = command.substr(0, command.find(' '));
        _modelManager->saveModel(modelName);
        _picker->saveMutableTextures();
    }
    else if (commandType == "CLEAR") {
        command               = command.substr(command.find(' ') + 1);
        std::string modelName = command.substr(0, command.find(' '));
        _modelManager->clearChanges(modelName);
    }
    else if (commandType == "UNDO") {
        //TODO: Undo previous change to scene
    }
}

void Terminal::processCommands() {
    
    if (_gameState.worldEditorModeEnabled) {

        glClear(GL_DEPTH_BUFFER_BIT);
        _fontRenderer.drawFont(0.0f,
                               -1.9f,
                               _commandString, 
                               0);
    }

    if (_commandToProcess.size() > 0) {
        _commandProcessor(_commandToProcess);
        // Clear out all commands
        _commandToProcess = "";
    }
}

bool Terminal::_mouseDeletion(Entity* entity) {

    if (_commandHistory.size() > 0) {

        auto commandString  = _commandHistory[_commandHistoryIndex];
        //removes all occurrences of '|'
        commandString.erase(std::remove(commandString.begin(),
                            commandString.end(), '|'),
                            commandString.end());

        auto location       = commandString.find(' ');
        std::string command = commandString.substr(0, location);

        if (command == "MOUSEDELETE") {

            commandString         = commandString.substr(commandString.find(' ') + 1);
            std::string modelName = commandString.substr(0, commandString.find(' '));

            _modelManager->removeModel(entity,
                                       modelName);
            return true;
        }
    }
    return false;
}

bool Terminal::_mousePosition(Vector4 position, bool mouseClick) {

    if (_commandHistory.size() > 0) {

        auto commandString  = _commandHistory[_commandHistoryIndex];
        //removes all occurrences of '|'
        commandString.erase(std::remove(commandString.begin(),
                            commandString.end(), '|'),
                            commandString.end());

        auto location       = commandString.find(' ');
        std::string command = commandString.substr(0, location);

        if (command == "MOUSEADD") {
            commandString          = commandString.substr(commandString.find(' ') + 1);
            std::string modelName  = commandString.substr(0, commandString.find(' '));
            commandString          = commandString.substr(commandString.find(' ') + 1);
            std::string modelToAdd = commandString.substr(0, commandString.find(' '));
            if (modelToAdd == "*") {
                auto modelNamesList = _modelManager->getModelNames();
                modelToAdd          = modelNamesList[_modelNameIndex];
            }
            //Rotation
            commandString    = commandString.substr(commandString.find(' ') + 1);
            std::string rotX = commandString.substr(0, commandString.find(' '));
            commandString    = commandString.substr(commandString.find(' ') + 1);
            std::string rotY = commandString.substr(0, commandString.find(' '));
            commandString    = commandString.substr(commandString.find(' ') + 1);
            std::string rotZ = commandString.substr(0, commandString.find(' '));
            Vector4 rot(static_cast<float>(atof(rotX.c_str())),
                        static_cast<float>(atof(rotY.c_str())),
                        static_cast<float>(atof(rotZ.c_str())));

            commandString               = commandString.substr(commandString.find(' ') + 1);
            std::string scale           = commandString.substr(0, commandString.find(' '));
            position.getFlatBuffer()[3] = static_cast<float>(atof(scale.c_str()));

            if (mouseClick) {
                _modelManager->addModel(modelName,
                                        modelToAdd,
                                        position,
                                        rot);
            }
            else {
                if (_modelManager->getModel(modelToAdd) != nullptr) {

                    auto transformation = Matrix::translation(position.getx(), position.gety(), position.getz()) *
                                          Matrix::rotationAroundY(rot.gety()) *
                                          Matrix::scale(position.getw()) *
                                          _modelManager->getModel(modelToAdd)->getFbxLoader()->getObjectSpaceTransform();

                    EngineManager::addEntity(ModelBroker::instance()->getModel(modelToAdd),
                                             transformation,
                                             true);
                }
            }
            return true;
        }
    }
    return false;
}

void Terminal::_updateKeyboard(int key, int x, int y) { //Do stuff based on keyboard update

    //Process command if entered correctly
    if (_gameState.worldEditorModeEnabled && key == GLFW_KEY_ENTER) {
        _commandToProcess    = _commandString;
        //removes all occurrences of '|'
        _commandToProcess.erase(std::remove(_commandToProcess.begin(),
                                _commandToProcess.end(), '|'),
                                _commandToProcess.end());
        _commandHistory.push_back(_commandToProcess);
        _commandHistoryIndex = _commandHistory.size() - 1;
    }

    if (_gameState.worldEditorModeEnabled &&
        key != GLFW_KEY_GRAVE_ACCENT &&
        key != GLFW_KEY_ENTER) {

        if (key == GLFW_KEY_LEFT || key == GLFW_KEY_RIGHT) {

            int i = _getCursorIndex();
            if ((i > 0 && key == GLFW_KEY_LEFT) ||
                (i < _commandString.size() - 1 && key == GLFW_KEY_RIGHT)) {
                if (key == GLFW_KEY_LEFT && !_shiftKeyPressed) {
                    auto temp = _commandString[i - 1];
                    _commandString[i - 1] = '|';
                    _commandString[i] = temp;
                }
                else if (key == GLFW_KEY_LEFT && _shiftKeyPressed) {
                    auto temp = _commandString[i - 1];
                    _commandString[i - 1] = '|';
                    _commandString[i] = temp;
                }
                else  if (key == GLFW_KEY_RIGHT && !_shiftKeyPressed) {
                    auto temp = _commandString[i + 1];
                    _commandString[i + 1] = '|';
                    _commandString[i] = temp;
                }
                else  if (key == GLFW_KEY_RIGHT && _shiftKeyPressed) {
                    auto temp = _commandString[i + 1];
                    _commandString[i + 1] = '|';
                    _commandString[i] = temp;
                }
            }
        }
        else if (key == GLFW_KEY_LEFT_SHIFT || key == GLFW_KEY_RIGHT_SHIFT) {
            _shiftKeyPressed = true;
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
            if (_commandHistory.size() > 0 && _commandHistoryIndex < _commandHistory.size() - 1) {
                _commandHistoryIndex++;
                _commandString = _commandHistory[_commandHistoryIndex];
            }
        }
        else {
            int i = _getCursorIndex();
            if (i != _commandString.size() - 1 && _commandString.size() > 1) {
                if (_shiftKeyPressed && key == 56) { //key stroke * or wildcard
                    key = 42;
                }
                _commandString = _commandString.substr(0, i) + char(key) + '|' + _commandString.substr(i + 1);
            }
            else {
                _commandString[i] = char(key);
                _commandString += '|';
            }
        }
    }
}

void Terminal::_updateReleaseKeyboard(int key, int x, int y) { //Do stuff based on keyboard release update

    //update id buffer hopefully after a frame update is made
    if (_gameState.worldEditorModeEnabled == true) {
        _picker->updateIdBuffer();
    }

    if (_gameState.worldEditorModeEnabled &&
        key != GLFW_KEY_GRAVE_ACCENT      &&
        key != GLFW_KEY_ENTER) {

        if (key == GLFW_KEY_LEFT_SHIFT ||
            key == GLFW_KEY_RIGHT_SHIFT) {
            _shiftKeyPressed = false;
        }
    }
}

void Terminal::_updateGameState(EngineStateFlags state) {
    
    //If we go into terminal mode cache the frame buffer's entity and triangle ids for picking efficiency
    if (state.worldEditorModeEnabled      == true &&
        _gameState.worldEditorModeEnabled != state.worldEditorModeEnabled)
    {
        _picker->updateIdBuffer();
    }
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