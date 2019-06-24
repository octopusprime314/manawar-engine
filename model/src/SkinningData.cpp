#include "SkinningData.h"
#include <iterator>
#include "VectorUtil.h"

SkinningData::SkinningData(FbxCluster* skinData, FbxNode* node, int animationFrames, int indexOffset) {

    int skinningPoints = skinData->GetControlPointIndicesCount();
    auto indices = skinData->GetControlPointIndices();
    auto weights = skinData->GetControlPointWeights();

    std::copy(&indices[0], &indices[skinningPoints], back_inserter(_indexes));
    //Safely converts double templated array type to float vector type
    VectorUtil::toType<double, float>(weights, skinningPoints, _weights);

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
        Matrix matrix;
        float* mat = matrix.getFlatBuffer();
        mat[0] =  static_cast<float>(matVertex[0] ), mat[1]  = static_cast<float>(matVertex[1] ), mat[2]  = static_cast<float>(matVertex[2] ), mat[3]  = static_cast<float>(matVertex[3] );
        mat[4] =  static_cast<float>(matVertex[4] ), mat[5]  = static_cast<float>(matVertex[5] ), mat[6]  = static_cast<float>(matVertex[6] ), mat[7]  = static_cast<float>(matVertex[7] );
        mat[8] =  static_cast<float>(matVertex[8] ), mat[9]  = static_cast<float>(matVertex[9] ), mat[10] = static_cast<float>(matVertex[10]), mat[11] = static_cast<float>(matVertex[11]);
        mat[12] = static_cast<float>(matVertex[12]), mat[13] = static_cast<float>(matVertex[13]), mat[14] = static_cast<float>(matVertex[14]), mat[15] = static_cast<float>(matVertex[15]);

        _frameVertexTransforms.push_back(matrix); //Add the transform
    }

    //Keeps track off the number of indices
    _indexOffset = indexOffset;
}

SkinningData::~SkinningData() {

}

std::vector<int>* SkinningData::getIndexes() {
    return &_indexes;
}
std::vector<float>* SkinningData::getWeights() {
    return &_weights;
}
std::vector<Matrix>* SkinningData::getFrameVertexTransforms() {
    return &_frameVertexTransforms;
}
int SkinningData::getIndexOffset() {
    return _indexOffset;
}