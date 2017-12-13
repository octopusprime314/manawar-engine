#include "SkinningData.h"
#include <iterator>

SkinningData::SkinningData(FbxCluster* skinData, FbxNode* node, int animationFrames, int indexOffset) {

    int skinningPoints = skinData->GetControlPointIndicesCount();
    auto indices = skinData->GetControlPointIndices();
    auto weights = skinData->GetControlPointWeights();

    std::copy(&indices[0], &indices[skinningPoints], back_inserter(_indexes));
    std::copy(&weights[0], &weights[skinningPoints], back_inserter(_weights));

    FbxVector4 T = node->GetGeometricTranslation(FbxNode::eSourcePivot);
    FbxVector4 R = node->GetGeometricRotation(FbxNode::eSourcePivot);
    FbxVector4 S = node->GetGeometricScaling(FbxNode::eSourcePivot);
    FbxAMatrix geometryTransform(T, R, S);
    FbxVector4 rotVector(R);

    FbxCluster::ELinkMode linkMode = FbxCluster::eNormalize;
    FbxAMatrix  transformMatrix, transformLinkMatrix;

    skinData->GetTransformMatrix(transformMatrix);
    skinData->GetTransformLinkMatrix(transformLinkMatrix);

    FbxAMatrix globalBindposeInverseMatrix = transformLinkMatrix.Inverse() * transformMatrix * geometryTransform;

    for (FbxLongLong j = 0; j < animationFrames; j++) {
        FbxTime currTime;
        currTime.SetFrame(j, FbxTime::eFrames60);

        //Multiply node offset with global transform at time currTime and finally with inverse bind pose transform
        FbxAMatrix currentTransformOffset = node->EvaluateGlobalTransform(currTime) * geometryTransform;

        FbxAMatrix transformed = ((currentTransformOffset.Inverse() * skinData->GetLink()->EvaluateGlobalTransform(currTime)
            * globalBindposeInverseMatrix).Transpose());

        double* matVertex = (double *)&transformed[0][0]; //Cast to linear double pointer
        _frameVertexTransforms.push_back(Matrix(matVertex)); //Add the transform
    }

    //Keeps track off the number of indices
    _indexOffset = indexOffset;
}

SkinningData::~SkinningData() {

}

std::vector<int>* SkinningData::getIndexes() {
    return &_indexes;
}
std::vector<double>* SkinningData::getWeights() {
    return &_weights;
}
std::vector<Matrix>* SkinningData::getFrameVertexTransforms() {
    return &_frameVertexTransforms;
}
int SkinningData::getIndexOffset(){
    return _indexOffset;
}