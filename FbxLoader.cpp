#include "FbxLoader.h"
#include "AnimatedModel.h"
#include "Model.h"
#include <map>
#include "Texture.h"
#include <algorithm>

FbxLoader::FbxLoader(std::string name) {

    _fbxManager = FbxManager::Create();
    if (_fbxManager == nullptr) {
        printf("ERROR %s : %d failed creating FBX Manager!\n", __FILE__, __LINE__);
    }

    _ioSettings = FbxIOSettings::Create(_fbxManager, IOSROOT);
    _fbxManager->SetIOSettings(_ioSettings);

    FbxString filePath = FbxGetApplicationDirectory();
    _fbxManager->LoadPluginsDirectory(filePath.Buffer());

    _scene = FbxScene::Create(_fbxManager, "");

    int fileMinor, fileRevision;
    int sdkMajor, sdkMinor, sdkRevision;
    int fileFormat;

    FbxManager::GetFileFormatVersion(sdkMajor, sdkMinor, sdkRevision);
    FbxImporter* importer = FbxImporter::Create(_fbxManager, "");

    if (!_fbxManager->GetIOPluginRegistry()->DetectReaderFileFormat(name.c_str(), fileFormat)) {
        //Unrecognizable file format. Try to fall back on FbxImorter::eFBX_BINARY
        fileFormat = _fbxManager->GetIOPluginRegistry()->FindReaderIDByDescription("FBX binary (*.fbx)");
    }

    bool importStatus = importer->Initialize(name.c_str(), fileFormat, _fbxManager->GetIOSettings());
    importer->GetFileVersion(fileMinor, fileMinor, fileRevision);

    if (!importStatus) {
        printf("ERROR %s : %d FbxImporter Initialize failed!\n", __FILE__, __LINE__);
    }

    importStatus = importer->Import(_scene);

    if (!importStatus) {
        printf("ERROR %s : %d FbxImporter failed to import the file to the scene!\n", __FILE__, __LINE__);
    }

    importer->Destroy();
}

FbxLoader::~FbxLoader() {
    _scene->Destroy();
    _ioSettings->Destroy();
    _fbxManager->Destroy();
}

FbxScene* FbxLoader::getScene() {
    return _scene;
}

void FbxLoader::loadModel(Model* model, FbxNode* node) {

    // Determine the number of children there are
    int numChildren = node->GetChildCount();
    FbxNode* childNode = nullptr;
    for (int i = 0; i < numChildren; i++) {
        childNode = node->GetChild(i);

        FbxMesh* mesh = childNode->GetMesh();
        if (mesh != nullptr) {
            loadModelData(model, mesh, childNode);
        }
        loadModel(model, childNode);
    }
}

void FbxLoader::loadAnimatedModel(AnimatedModel* model, FbxNode* node) {

    // Determine the number of children there are
    int numChildren = node->GetChildCount();
    FbxNode* childNode = nullptr;
    for (int i = 0; i < numChildren; i++) {
        childNode = node->GetChild(i);

        FbxMesh* mesh = childNode->GetMesh();
        if (mesh != nullptr) {
            //Look for a skin animation
            int deformerCount = mesh->GetDeformerCount();

            FbxDeformer* pFBXDeformer;
            FbxSkin*     pFBXSkin;

            for (int i = 0; i < deformerCount; ++i) {
                pFBXDeformer = mesh->GetDeformer(i);

                if (pFBXDeformer == nullptr || pFBXDeformer->GetDeformerType() != FbxDeformer::eSkin) {
                    continue;
                }

                pFBXSkin = (FbxSkin*)(pFBXDeformer);
                if (pFBXSkin == nullptr) {
                    continue;
                }
                loadAnimatedModelData(model, pFBXSkin, node, mesh);
            }
        }
        loadAnimatedModel(model, childNode);
    }

}

void FbxLoader::loadAnimatedModelData(AnimatedModel* model, FbxSkin* pSkin, FbxNode* node, FbxMesh* mesh) {

    //Used to supply animation frame data for skinning a model
    std::vector<SkinningData> skins;

    //Load the global animation stack information
    FbxAnimStack* currAnimStack = node->GetScene()->GetSrcObject<FbxAnimStack>(0);
    FbxString animStackName = currAnimStack->GetName();
    FbxTakeInfo* takeInfo = node->GetScene()->GetTakeInfo(animStackName);
    FbxTime start = takeInfo->mLocalTimeSpan.GetStart();
    FbxTime end = takeInfo->mLocalTimeSpan.GetStop();
    int animationFrames = (int)(end.GetFrameCount(FbxTime::eFrames60) - start.GetFrameCount(FbxTime::eFrames60) + 1);
 
    auto animation = model->getAnimation();
    animation->setFrames(animationFrames); //Set the number of frames this animation contains
    auto previousSkins = animation->getSkins();
    int indexOffset = 0;
    if(previousSkins.size() > 0){
        indexOffset = previousSkins.back().getIndexOffset();
    }

    int clusterCount = pSkin->GetClusterCount();
    for (int i = 0; i < clusterCount; ++i) {

        auto pCluster = pSkin->GetCluster(i);
        if (pCluster == nullptr) {
            continue;
        }

        auto pLinkNode = pCluster->GetLink();
        if (pLinkNode == nullptr) {
            continue;
        }
        skins.push_back(SkinningData(pCluster, node, animationFrames, mesh->GetControlPointsCount()));
    }

    if(model->getClassType() == ModelClass::AnimatedModelType){

        if(animation->getSkins().size() > 0) {
            for(SkinningData& skin : skins){
                auto ind = skin.getIndexes();
                transform(ind->begin(), ind->end(), ind->begin(), bind2nd(std::plus<int>(), indexOffset));     
            }
        }
    }
    animation->addSkin(skins);
}

void FbxLoader::buildAnimationFrames(AnimatedModel* model, std::vector<SkinningData>& skins) {

    Animation* animation = model->getAnimation();
    std::vector<Vector4>* vertices = model->getVertices(); //Get the reference to an object to prevent copy
    size_t verticesSize = vertices->size();
    std::vector<int>* indices = model->getIndices();
    size_t indicesSize = indices->size();
    int animationFrames = animation->getFrameCount();

    std::vector<std::vector<int>>* boneIndexes = new std::vector<std::vector<int>>(verticesSize); //Bone indexes per vertex
    std::vector<std::vector<float>>* boneWeights = new std::vector<std::vector<float>>(verticesSize); //Bone weights per vertex
    int boneIndex = 0;
    for (SkinningData skin : skins) { //Dereference vertices
        std::vector<int>*    indexes = skin.getIndexes(); //Get all of the vertex indexes that are affected by this skinning data
        std::vector<double>* weights = skin.getWeights(); //Get all of the vertex weights that are affected by this skinning data
        int subSkinIndex = 0;
        for (int index : *indexes) { //Derefence indexes
                
            (*boneIndexes)[index].push_back(boneIndex);
            (*boneWeights)[index].push_back((*weights)[subSkinIndex]);
            ++subSkinIndex; //Indexer for other vectors
        }
        boneIndex++;
    }
    
    //Pad data
    for(int i = 0; i < boneWeights->size(); i++){
        if((*boneWeights)[i].size() > 4){
            std::cout << "Currently cannot support this animation model!" << std::endl;
        }
        (*boneWeights)[i].resize(4);
        (*boneIndexes)[i].resize(4);
    }

    animation->setBoneIndexWeights(boneIndexes, boneWeights);

    for (int animationFrame = 0; animationFrame < animationFrames; ++animationFrame) {

        std::vector<Matrix>* bones = new std::vector<Matrix>(); //Bone weights per vertex
        for (SkinningData skin : skins) { //Dereference vertices
            std::vector<Matrix>* vertexTransforms = skin.getFrameVertexTransforms(); //Get all of the vertex transform data per frame
            bones->push_back((*vertexTransforms)[animationFrame]);
        }
        animation->addBoneTransforms(bones); //Add a new frame to the animation and do not store in GPU memory yet
    }
}


void FbxLoader::loadModelData(Model* model, FbxMesh* meshNode, FbxNode* childNode) {

    //Get the indices from the mesh
    int* indices = nullptr;
    _loadIndices(model, meshNode, indices);

    //Get the vertices from the mesh
    std::vector<Vector4> vertices;
    _loadVertices(meshNode, vertices);

    //Get the normals from the mesh
    std::vector<Vector4> normals;
    _loadNormals(meshNode, indices, normals);

    //Get the textures UV/ST coordinates from the mesh
    std::vector<Texture2> textures;
    _loadTextureUVs(meshNode, textures);

    //Send texture image data to GPU
    _loadTextures(model, meshNode, childNode);

    //Build the entire model in one long stream of vertex, normal and texture buffers
    _buildModelData(model, meshNode, childNode, vertices, normals, textures);
}

void FbxLoader::_loadIndices(Model* model, FbxMesh* meshNode, int*& indices){
    
    //Get the indices from the model
    int  numIndices = meshNode->GetPolygonVertexCount();
    indices = meshNode->GetPolygonVertices();

    if(model->getClassType() == ModelClass::ModelType){
        model->setVertexIndices(std::vector<int>(indices, indices + numIndices)); //Copy vector
    }
    else if(model->getClassType() == ModelClass::AnimatedModelType){
        std::vector<int> newIndices(indices, indices + numIndices);

        //Find previous maximum vertex index and add to all of the sequential indexes
        auto currentIndices = model->getIndices();
        if(currentIndices->size() > 0){
            auto maxIndex = std::max_element(currentIndices->begin(), currentIndices->end());
            //Add one to the maxIndex otherwise this model uses the last vertex from the previous model!!!!!!!!!!!!!
            transform(newIndices.begin(), newIndices.end(), newIndices.begin(), bind2nd(std::plus<int>(), (*maxIndex) + 1));     
        }
        model->addVertexIndices(newIndices); //Copy vector
    }
}

void FbxLoader::_loadVertices(FbxMesh* meshNode, std::vector<Vector4>& vertices){
    //Get the vertices from the model
    int  numVerts = meshNode->GetControlPointsCount();
    for (int j = 0; j < numVerts; j++) {
        FbxVector4 coord = meshNode->GetControlPointAt(j);
        vertices.push_back(Vector4((float)coord.mData[0], (float)coord.mData[1], (float)coord.mData[2], 1.0));
    }
}

void FbxLoader::_buildModelData(Model* model, FbxMesh* meshNode, FbxNode* childNode, std::vector<Vector4>& vertices, 
                                std::vector<Vector4>& normals, std::vector<Texture2>& textures) {
    int numVerts = meshNode->GetControlPointsCount();
    //Load in models differently based on type
    if (model->getClassType() == ModelClass::AnimatedModelType) {
        //Load vertices and normals into model
        for (int i = 0; i < numVerts; i++) {
            model->addVertex(vertices[i]); 
            model->addNormal(normals[i]); 
            model->addDebugNormal(vertices[i]);
            model->addDebugNormal(vertices[i] + normals[i]);
        }
        //Load texture coordinates
        for (int i = 0; i < textures.size(); i++) {
            model->addTexture(textures[i]);
        }
    }
    else if (model->getClassType() == ModelClass::ModelType) {
        //Load in the entire model once if the data is not animated
        _buildTriangles(model, vertices, normals, textures, *model->getIndices(), childNode);
    }
}

void FbxLoader::_loadNormals(FbxMesh* meshNode, int* indices, std::vector<Vector4>& normals){
    std::map<int, Vector4> mappingNormals;
    FbxGeometryElementNormal* normalElement = meshNode->GetElementNormal();
    const FbxLayerElement::EMappingMode mapMode = normalElement->GetMappingMode();
    if (normalElement) {
        if (mapMode == FbxLayerElement::EMappingMode::eByPolygonVertex) {
            int numNormals = meshNode->GetPolygonCount() * 3;

            // Loop through the triangle meshes
            for (int polyCounter = 0; polyCounter < numNormals; ++polyCounter) {
                //Get the normal for this vertex
                FbxVector4 normal = normalElement->GetDirectArray().GetAt(polyCounter);
                mappingNormals[indices[polyCounter]] = Vector4((float)normal[0], (float)normal[1], (float)normal[2], 1.0f);
            }

            for (int j = 0; j < mappingNormals.size(); j++) {
                FbxVector4 coord = meshNode->GetControlPointAt(j);
                normals.push_back(Vector4(mappingNormals[j].getx(), mappingNormals[j].gety(), mappingNormals[j].getz(), 1.0));
            }
        }
        else if (mapMode == FbxLayerElement::EMappingMode::eByControlPoint) {
            //Let's get normals of each vertex, since the mapping mode of normal element is by control point
            for (int vertexIndex = 0; vertexIndex < meshNode->GetControlPointsCount(); ++vertexIndex) {
                int normalIndex = 0;
                //reference mode is direct, the normal index is same as vertex index.
                //get normals by the index of control vertex
                if (normalElement->GetReferenceMode() == FbxGeometryElement::eDirect) {
                    normalIndex = vertexIndex;
                }

                //reference mode is index-to-direct, get normals by the index-to-direct
                if (normalElement->GetReferenceMode() == FbxGeometryElement::eIndexToDirect) {
                    normalIndex = normalElement->GetIndexArray().GetAt(vertexIndex);
                }

                //Got normals of each vertex.
                FbxVector4 normal = normalElement->GetDirectArray().GetAt(normalIndex);
                normals.push_back(Vector4((float)normal[0], (float)normal[1], (float)normal[2], 1.0));
            }
        }
    }
}

void FbxLoader::_loadTextureUVs(FbxMesh* meshNode, std::vector<Texture2>& textures){

    int numIndices = meshNode->GetPolygonVertexCount();

    //Get the texture coordinates from the mesh
	FbxVector2 uv;
	FbxGeometryElementUV*                 leUV = meshNode->GetElementUV( 0 );
	const FbxLayerElement::EReferenceMode refMode = leUV->GetReferenceMode();
	const FbxLayerElement::EMappingMode   textMapMode = leUV->GetMappingMode();
	if(refMode == FbxLayerElement::EReferenceMode::eDirect) {
		int numTextures = meshNode->GetTextureUVCount();
		// Loop through the triangle meshes
		for(int textureCounter = 0; textureCounter < numTextures; textureCounter++) {
			//Get the normal for this vertex
			FbxVector2 uvTexture = leUV->GetDirectArray().GetAt(textureCounter);
            textures.push_back(Texture2(uvTexture[0], uvTexture[1]));
		}
	}
	else if(refMode == FbxLayerElement::EReferenceMode::eIndexToDirect) {
		int indexUV;
		for(int i = 0; i < numIndices; i++) {
			//Get the normal for this vertex
			indexUV = leUV->GetIndexArray().GetAt(i);
			FbxVector2 uvTexture = leUV->GetDirectArray().GetAt(indexUV);
			textures.push_back(Texture2(uvTexture[0], uvTexture[1]));
		}
	}
}

void FbxLoader::_generateTextureStrides(FbxMesh* meshNode, std::vector<int>& textureStrides){
    //Get material element info
    FbxLayerElementMaterial* pLayerMaterial = meshNode->GetLayer(0)->GetMaterials();   
    //Get material mapping info
	FbxLayerElementArrayTemplate<int> *tmpArray = &pLayerMaterial->GetIndexArray();   
    //Get mapping mode
    FbxLayerElement::EMappingMode mapMode = pLayerMaterial->GetMappingMode();

	int textureCountage = tmpArray->GetCount();

    if(mapMode == FbxLayerElement::EMappingMode::eAllSame){
        textureStrides.push_back(meshNode->GetPolygonVertexCount());
    }
    else{
        int currMaterial;
        int vertexStride = 0;
	    for(int i = 0; i < textureCountage; i++) {     
		    if(i == 0) {
			    currMaterial = tmpArray->GetAt(i);
		    }
		    else {
			    if(currMaterial != tmpArray->GetAt(i)) {
				    currMaterial = tmpArray->GetAt(i);
                    if(mapMode == FbxLayerElement::EMappingMode::eByPolygon){
                        textureStrides.push_back(vertexStride*3); //Multiply by 3 because materials are done per triangle not per vertex
                    }
                    else if(mapMode == FbxLayerElement::EMappingMode::eByPolygonVertex){
                        textureStrides.push_back(vertexStride);
                    }
                    vertexStride = 0;
			    }
		    }
            vertexStride++;
	    }
        if(mapMode == FbxLayerElement::EMappingMode::eByPolygon){
            textureStrides.push_back(vertexStride*3); //Multiply by 3 because materials are done per triangle not per vertex
        }
        else if(mapMode == FbxLayerElement::EMappingMode::eByPolygonVertex){
            textureStrides.push_back(vertexStride);
        }
    }
}

bool FbxLoader::_loadTexture(Model* model, int textureStride, FbxFileTexture* textureFbx, int textureIndex){

    if(textureFbx != nullptr){
		// Then, you can get all the properties of the texture, including its name
		std::string textureName = textureFbx->GetFileName();
        std::string textureNameTemp = textureName; //Used a temporary storage to not overwrite textureName
        std::string texturePath = "..\\models\\textures";
        //Finds second to last position of string and use that for file access name
		texturePath.append(textureName.substr(textureNameTemp.substr(0, textureNameTemp.find_last_of("/\\")).find_last_of("/\\")));
        model->addTexture(texturePath, textureStride);
        return true;
    }
    return false;
}

void FbxLoader::_loadTextures(Model* model, FbxMesh* meshNode, FbxNode* childNode) {
    
    //First verify if mesh nodes has materials...
    if(meshNode->GetLayerCount() == 0){
        return;
    }
    std::vector<int> strides;
    _generateTextureStrides(meshNode, strides);

	//Return the number of materials found in mesh
	int materialCount = childNode->GetSrcObjectCount<FbxSurfaceMaterial>();
    int textureStrideIndex = 0;
	for (int materialIndex = 0; materialIndex < materialCount; ++materialIndex) {
		
        FbxSurfaceMaterial* material = (FbxSurfaceMaterial*)childNode->GetSrcObject<FbxSurfaceMaterial>(materialIndex);
		if (material == nullptr) {
            continue;
        }

		// This only gets the material of type sDiffuse,
        // you probably need to traverse all Standard Material Property by its name to get all possible textures.
		FbxProperty propDiffuse = material->FindProperty(FbxSurfaceMaterial::sDiffuse);

        // Check if it's layeredtextures
	    int layeredTextureCount = propDiffuse.GetSrcObjectCount<FbxLayeredTexture>();

        if (layeredTextureCount > 0) {
            for (int j = 0; j < layeredTextureCount; j++) {
				FbxLayeredTexture* layered_texture = FbxCast<FbxLayeredTexture>(propDiffuse.GetSrcObject<FbxLayeredTexture>(j));
                if(layered_texture != nullptr) {

                    int textureCount = layered_texture->GetSrcObjectCount<FbxTexture>();
                    for (int textureIndex = 0; textureIndex < textureCount; textureIndex++) {

                        //Fetch the diffuse texture
                        FbxFileTexture* textureFbx = FbxCast<FbxFileTexture >(layered_texture->GetSrcObject<FbxFileTexture >(textureIndex));
                        if(_loadTexture(model, strides[textureStrideIndex], textureFbx, textureIndex)){
                            textureStrideIndex++;
                        }

                        // TODO Implement layered textures but for now just grab the first texture and break out
                        break;
                    }
                }
            }
        }
        else {
		    // Get number of textures in the material which could contain diffuse, bump, normal and/or specular maps...
            int textureCount = propDiffuse.GetSrcObjectCount<FbxTexture>();

		    for (int textureIndex = 0; textureIndex < textureCount; ++textureIndex) {
                
                //Fetch the diffuse texture
                FbxFileTexture* textureFbx = FbxCast<FbxFileTexture>(propDiffuse.GetSrcObject<FbxFileTexture>(textureIndex));
                if(_loadTexture(model, strides[textureStrideIndex], textureFbx, textureIndex)){
                    textureStrideIndex++;
                }
		    }
        }
	}
}

void FbxLoader::_buildTriangles(Model* model, std::vector<Vector4>& vertices, std::vector<Vector4>& normals,
    std::vector<Texture2>& textures, std::vector<int>& indices, FbxNode* node) {

    //Global transform
    FbxDouble* TBuff = node->EvaluateGlobalTransform().GetT().Buffer();
    FbxDouble* RBuff = node->EvaluateGlobalTransform().GetR().Buffer();
    FbxDouble* SBuff = node->EvaluateGlobalTransform().GetS().Buffer();

    //Compute x, y and z rotation vectors by multiplying through
    Matrix rotation = Matrix::rotationAroundX(RBuff[0]) * Matrix::rotationAroundY(RBuff[1]) * Matrix::rotationAroundZ(RBuff[2]);
    Matrix translation = Matrix::translation(TBuff[0], TBuff[1], TBuff[2]);
    Matrix scale = Matrix::scale(SBuff[0], SBuff[1], SBuff[2]);

    int triCount = 0;
    Matrix transformation = translation * rotation * scale;

    size_t totalTriangles = indices.size() / 3; //Each index represents one vertex and a triangle is 3 vertices
    //Read each triangle vertex indices and store them in triangle array
    for (int i = 0; i < totalTriangles; i++) {
        Vector4 A(vertices[indices[triCount]].getx(),
            vertices[indices[triCount]].gety(),
            vertices[indices[triCount]].getz(),
            1.0);
        model->addVertex(transformation * A); //Scale then rotate vertex

        Vector4 AN(normals[indices[triCount]].getx(),
            normals[indices[triCount]].gety(),
            normals[indices[triCount]].getz(),
            1.0);
        model->addNormal(rotation * AN); //Scale then rotate normal
        model->addTexture(textures[triCount]);
        model->addDebugNormal(transformation * A);
        model->addDebugNormal((transformation * A) + (rotation * AN));
        triCount++;

        Vector4 B(vertices[indices[triCount]].getx(),
            vertices[indices[triCount]].gety(),
            vertices[indices[triCount]].getz(),
            1.0);
        model->addVertex(transformation * B); //Scale then rotate vertex

        Vector4 BN(normals[indices[triCount]].getx(),
            normals[indices[triCount]].gety(),
            normals[indices[triCount]].getz(),
            1.0);
        model->addNormal(rotation * BN); //Scale then rotate normal
        model->addTexture(textures[triCount]);
        model->addDebugNormal(transformation * B);
        model->addDebugNormal((transformation * B) + (rotation * BN));
        triCount++;

        Vector4 C(vertices[indices[triCount]].getx(),
            vertices[indices[triCount]].gety(),
            vertices[indices[triCount]].getz(),
            1.0);
        model->addVertex(transformation * C); //Scale then rotate vertex

        Vector4 CN(normals[indices[triCount]].getx(),
            normals[indices[triCount]].gety(),
            normals[indices[triCount]].getz(),
            1.0);
        model->addNormal(rotation * CN); //Scale then rotate normal
        model->addTexture(textures[triCount]);
        model->addDebugNormal(transformation * C);
        model->addDebugNormal((transformation * C) + (rotation * CN));
        triCount++;
    }
}

