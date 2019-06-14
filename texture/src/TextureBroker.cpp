#include "TextureBroker.h"
#include "EngineManager.h"
#include "DXLayer.h"
TextureBroker* TextureBroker::_broker = nullptr;

TextureBroker* TextureBroker::instance() { //Only initializes the static pointer once
    if (_broker == nullptr) {
        _broker = new TextureBroker();
    }
    return _broker;
}
TextureBroker::TextureBroker() {

}
TextureBroker::~TextureBroker() {

}

void TextureBroker::addTexture(std::string textureName) {

    if (_textures.find(textureName) == _textures.end()) {
        if (EngineManager::getGraphicsLayer() == GraphicsLayer::OPENGL) {
            _textures[textureName] = new AssetTexture(textureName);
        }
        else if (EngineManager::getGraphicsLayer() == GraphicsLayer::DX12) {
            _textures[textureName] = new AssetTexture(textureName, 
                DXLayer::instance()->getCmdList(), DXLayer::instance()->getDevice());
        }
    }
}

void TextureBroker::addLayeredTexture(std::vector<std::string> textureNames) {

    std::string sumString = "Layered";
    for (auto& str : textureNames) {
        sumString += str;
    }
    if (_layeredTextures.find(sumString) == _layeredTextures.end()) {

        std::vector<AssetTexture*> textures;

        if (EngineManager::getGraphicsLayer() == GraphicsLayer::OPENGL) {

            //prime the asset textures
            for (auto textureName : textureNames) {
                if (getTexture(textureName) == nullptr) {
                    addTexture(textureName);
                }
                textures.push_back(getTexture(textureName));
            }

            _layeredTextures[sumString] = new LayeredTexture(textures);
        }
        else if (EngineManager::getGraphicsLayer() == GraphicsLayer::DX12) {

            //prime the asset textures
            for (auto textureName : textureNames) {
                if (getTexture(textureName) == nullptr) {
                    addTexture(textureName);
                }
                textures.push_back(getTexture(textureName));
            }

            _layeredTextures[sumString] = new LayeredTexture(textures,
                DXLayer::instance()->getCmdList(), DXLayer::instance()->getDevice());
        }
    }
}

void TextureBroker::addCubeTexture(std::string textureName) {
    if (_textures.find(textureName) == _textures.end()) {
        if (EngineManager::getGraphicsLayer() == GraphicsLayer::OPENGL) {
            _textures[textureName] = new AssetTexture(textureName, true);
        }
        else if (EngineManager::getGraphicsLayer() == GraphicsLayer::DX12) {
            _textures[textureName] = new AssetTexture(textureName,
                DXLayer::instance()->getCmdList(), DXLayer::instance()->getDevice(), true);
        }
    }
}

AssetTexture* TextureBroker::getTexture(std::string textureName) {
    if (_textures.find(textureName) != _textures.end()) {
        return _textures[textureName];
    }
    else {
        return nullptr;
    }
}

LayeredTexture* TextureBroker::getLayeredTexture(std::string textureName) {
    
    if (_layeredTextures.find(textureName) != _layeredTextures.end()) {
        return _layeredTextures[textureName];
    }
    else {
        return nullptr;
    }
}

AssetTexture* TextureBroker::getAssetTextureFromLayered(std::string textureName) {

    for (auto& layeredTexture : _layeredTextures) {

        auto assetTextures = layeredTexture.second->getTextures();
        for (auto assetTexture : assetTextures) {
            if (assetTexture->getName().find(textureName) != std::string::npos) {
                return assetTexture;
            }
        }
    }
    return nullptr;
}

void TextureBroker::updateTextureToLayered(std::string textureName, FIBITMAP* pixelData) {
    
    for (auto& layeredTexture : _layeredTextures) {

        auto& assetTextures = layeredTexture.second->getTextures();
        
        for (auto& assetTexture : assetTextures) {

            if (assetTexture->getName().compare(textureName) == 0) {

                if (EngineManager::getGraphicsLayer() == GraphicsLayer::OPENGL) {
                    layeredTexture.second->updateTexture(textureName, pixelData);
                }
                else if (EngineManager::getGraphicsLayer() == GraphicsLayer::DX12) {
                    layeredTexture.second->setTexture(new AssetTexture(textureName,
                        DXLayer::instance()->getCmdList(), DXLayer::instance()->getDevice()));
                }
            }
        }
    }
}