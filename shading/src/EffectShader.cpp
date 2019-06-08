#include "EffectShader.h"
#include "Light.h"
#include "Water.h"
#include "ViewEventDistributor.h"
#include "GLSLShader.h"
#include "HLSLShader.h"
#include "EngineManager.h"

EffectShader::EffectShader(std::string shaderName) {
    
    if (EngineManager::getGraphicsLayer() == GraphicsLayer::OPENGL) {
        _shader = new GLSLShader(shaderName);
        glGenVertexArrays(1, &_vaoContext);
    }
    else {
        std::vector<DXGI_FORMAT>* formats = new std::vector<DXGI_FORMAT>();
        formats->push_back(DXGI_FORMAT_R8G8B8A8_UNORM);
        formats->push_back(DXGI_FORMAT_D32_FLOAT);
        _shader = new HLSLShader(shaderName, "", formats);
    }
}

EffectShader::~EffectShader() {

}

void EffectShader::runShader(Effect* effectObject, float seconds) {

    //LOAD IN SHADER
    _shader->bind();

    if (EngineManager::getGraphicsLayer() == GraphicsLayer::OPENGL) {
        glBindVertexArray(_vaoContext);
    }
    else {
        _shader->bindAttributes(nullptr);
    }
    
    auto cameraMVP = effectObject->getCameraMVP();

    auto view = cameraMVP.getViewMatrix();
    _shader->updateData("view", view.getFlatBuffer());

    auto projection = cameraMVP.getProjectionMatrix();
    _shader->updateData("projection", projection.getFlatBuffer());

    if (effectObject->getType() == EffectType::Fire || effectObject->getType() == EffectType::Smoke) {

        Light* light = static_cast<Light*>(effectObject);
       
        auto planeScale = Matrix::scale(2.0f);
        if (effectObject->getType() == EffectType::Smoke) {
            seconds /= 4.0f;
            planeScale = Matrix::scale(100.0f);
        }
        if (effectObject->getType() == EffectType::Fire) {
            seconds /= 2.0f;
            planeScale = Matrix::scale(120.0f);
        }
        auto lightMVP = light->getLightMVP();

        //Pass the type of fire to the shader to simulate i.e. candle light or bon fire
        int fireType = 2;
        _shader->updateData("fireType", &fireType);

        float* color = light->getColor().getFlatBuffer();
        _shader->updateData("fireColor", color);

        auto viewNoTrans = cameraMVP.getViewMatrix();
        viewNoTrans.getFlatBuffer()[3] = 0.0;
        viewNoTrans.getFlatBuffer()[7] = 0.0;
        viewNoTrans.getFlatBuffer()[11] = 0.0;

        auto model = lightMVP.getModelMatrix() * planeScale;
        _shader->updateData("model", model.getFlatBuffer());

        auto inverseViewNoTrans = viewNoTrans.inverse();
        _shader->updateData("inverseViewNoTrans", inverseViewNoTrans.getFlatBuffer());
    }
    else if (effectObject->getType() == EffectType::Water) {

        Water* water = static_cast<Water*>(effectObject);

        auto model = Matrix::rotationAroundX(-90.0f)
            * Matrix::translation(20.0f, -40.0f, 1.0f)
            * Matrix::scale(300.0f); //add to z component of translation to lower water line.

        _shader->updateData("model", model.getFlatBuffer());

        auto normalMatrix = (view * model).inverse().transpose();
        _shader->updateData("normal", normalMatrix.getFlatBuffer());

        _shader->updateData("noiseTexture", GL_TEXTURE0, water->getNoiseTexture());
    }

    //Pass game time to shader
    _shader->updateData("time", &seconds);

    if (EngineManager::getGraphicsLayer() == GraphicsLayer::OPENGL) {
        glDrawArrays(GL_TRIANGLE_STRIP, 0, (GLsizei)4);
    }
    else {
        _shader->draw(0, 1, 4);
    }

    _shader->unbindAttributes();

    _shader->unbind();
}