#include "FbxLoader.h"
#include "AnimatedModel.h"
#include "Model.h"
#include <map>
#include "Texture.h"

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
            int ctrlPointCount = mesh->GetControlPointsCount();
            int deformerCount = mesh->GetDeformerCount();

            FbxDeformer* pFBXDeformer;
            FbxSkin*     pFBXSkin;

            for (int i = 0; i < deformerCount; ++i) {
                pFBXDeformer = mesh->GetDeformer(i);

                if (pFBXDeformer == nullptr) {
                    continue;
                }
                if (pFBXDeformer->GetDeformerType() != FbxDeformer::eSkin) {
                    continue;
                }
                pFBXSkin = (FbxSkin*)(pFBXDeformer);
                if (pFBXSkin == nullptr) {
                    continue;
                }
                loadAnimatedModelData(model, pFBXSkin, node);
            }
        }
        loadAnimatedModel(model, childNode);
    }
}

void FbxLoader::loadAnimatedModelData(AnimatedModel* model, FbxSkin* pSkin, FbxNode* node) {

    //First create an animation object
    Animation* animation = AnimationBuilder::buildAnimation();

    //Used to supply animation frame data for skinning a model
    std::vector<SkinningData> skins;

    //Load the global animation stack information
    FbxAnimStack* currAnimStack = node->GetScene()->GetSrcObject<FbxAnimStack>(0);
    FbxString animStackName = currAnimStack->GetName();
    std::string mAnimationName = animStackName.Buffer();
    FbxTakeInfo* takeInfo = node->GetScene()->GetTakeInfo(animStackName);
    FbxTime start = takeInfo->mLocalTimeSpan.GetStart();
    FbxTime end = takeInfo->mLocalTimeSpan.GetStop();
    int animationFrames = (int)(end.GetFrameCount(FbxTime::eFrames30) - start.GetFrameCount(FbxTime::eFrames30) + 1);
    FbxTime frameTime = takeInfo->mLocalTimeSpan.GetDuration();
    FbxTime::EMode mode = frameTime.GetGlobalTimeMode();
    int fps = (int)frameTime.GetFrameRate(mode);
    int animationDuration = (((float)fps*fps) / (FRAMERATE))*((frameTime.GetMilliSeconds() / 1000.0f) / ((float)animationFrames));

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
        skins.push_back(SkinningData(pCluster, node, animationFrames));
    }

    animation->setFrames(animationFrames); //Set the number of frames this animation contains
    model->addAnimation(animation); //Push animation onto the animated model
    _buildAnimationFrames(model, skins); //Build up each animation frame's vertices and normals
}

void FbxLoader::_buildAnimationFrames(AnimatedModel* model, const std::vector<SkinningData>& skins) {

    Animation* animation = model->getAnimation();
    std::vector<Vector4>* vertices = model->getVertices(); //Get the reference to an object to prevent copy
    std::vector<Vector4>* normals = model->getNormals(); //Get the reference to an object to prevent copy
    size_t verticesSize = vertices->size();
    std::vector<int>* indices = model->getIndices();
    size_t indicesSize = indices->size();
    int animationFrames = animation->getFrameCount();

    for (int animationFrame = 0; animationFrame < animationFrames; ++animationFrame) {

        std::vector<Vector4>* animationVertices = new std::vector<Vector4>(indicesSize); //Allocate a vertices pointer and pass it to AniminatedModel object
        std::vector<Vector4>* animationNormals = new std::vector<Vector4>(indicesSize); //Allocate a normals pointer and pass it to AniminatedModel object
        std::vector<Vector4>* animationDebugNormals = new std::vector<Vector4>(indicesSize * 2); //Allocate a normals debug pointer and pass it to AniminatedModel object
        std::vector<Matrix>   vertexWeightedTransformations(verticesSize); //Create on stack because only temporary
        std::vector<Matrix>   normalWeightedTransformations(verticesSize); //Create on stack because only temporary
        std::vector<bool>     affectedVertices(verticesSize, false); //Initialize a dirty bit indicator whether a vertex is affected by skin data

        //Build up weighted vertex animation transforms
        for (SkinningData skin : skins) { //Dereference vertices
            std::vector<int>*    indexes = skin.getIndexes(); //Get all of the vertex indexes that are affected by this skinning data
            std::vector<double>* weights = skin.getWeights(); //Get all of the vertex weights that are affected by this skinning data
            std::vector<Matrix>* vertexTransforms = skin.getFrameVertexTransforms(); //Get all of the vertex transform data per frame
            std::vector<Matrix>* normalTransforms = skin.getFrameNormalTransforms(); //Get all of the normal transform data per frame
            int subSkinIndex = 0;
            for (int index : *indexes) { //Derefence indexes
                if (!affectedVertices[index]) { //If vertex has not been flagged already for being affected by this skinning data
                    affectedVertices[index] = true;
                    vertexWeightedTransformations[index] = (*vertexTransforms)[animationFrame] * (*weights)[subSkinIndex];
                    normalWeightedTransformations[index] = (*normalTransforms)[animationFrame] * (*weights)[subSkinIndex];
                }
                else {
                    Matrix vertexWeightedTransform = (*vertexTransforms)[animationFrame] * (*weights)[subSkinIndex];
                    //Add on transform to pre existing vertex transform
                    vertexWeightedTransformations[index] = vertexWeightedTransformations[index] + vertexWeightedTransform;

                    Matrix normalWeightedTransform = (*normalTransforms)[animationFrame] * (*weights)[subSkinIndex];
                    //Add on transform to pre existing noraml transform
                    normalWeightedTransformations[index] = normalWeightedTransformations[index] + normalWeightedTransform;

                }
                ++subSkinIndex; //Indexer for other vectors
            }
        }

        //Transform vertices with vertex weighted transforms
        int weightedTransformIndex = 0;
        for (int index : *indices) {
            (*animationVertices)[weightedTransformIndex] = vertexWeightedTransformations[index] * (*vertices)[index];
            ++weightedTransformIndex;
        }
        //Transform normals with normal weighted transforms
        weightedTransformIndex = 0;
        for (int index : *indices) {
            (*animationNormals)[weightedTransformIndex] = normalWeightedTransformations[index] * (*normals)[index];
            (*animationNormals)[weightedTransformIndex].normalize(); //normalize the normal vector
            ++weightedTransformIndex;
        }
        //Build up debug normals
        weightedTransformIndex = 0;
        int vertexNormalIndex = 0;
        for (int index : *indices) {
            (*animationDebugNormals)[weightedTransformIndex] = (*animationVertices)[vertexNormalIndex];
            ++weightedTransformIndex;
            (*animationDebugNormals)[weightedTransformIndex] = (*animationVertices)[vertexNormalIndex] +
                (*animationNormals)[vertexNormalIndex];
            ++weightedTransformIndex;
            ++vertexNormalIndex;
        }
        //Create an animation frame and add it to the animation object for safe keeping!
        AnimationFrame* frame = new AnimationFrame(animationVertices, animationNormals, animationDebugNormals);
        animation->addFrame(frame, false); //Add a new frame to the animation and do not store in GPU memory yet
    }
}


void FbxLoader::loadModelData(Model* model, FbxMesh* meshNode, FbxNode* childNode) {

    //Global transform
    FbxDouble* TBuff = childNode->EvaluateGlobalTransform().GetT().Buffer();
    FbxDouble* RBuff = childNode->EvaluateGlobalTransform().GetR().Buffer();
    FbxDouble* SBuff = childNode->EvaluateGlobalTransform().GetS().Buffer();

    //Compute x, y and z rotation vectors by multiplying through
    Matrix rotation = Matrix::rotationAroundX(RBuff[0]) * Matrix::rotationAroundY(RBuff[1]) * Matrix::rotationAroundZ(RBuff[2]);
    Matrix translation = Matrix::translation(TBuff[0], TBuff[1], TBuff[2]);
    Matrix scale = Matrix::scale(SBuff[0], SBuff[1], SBuff[2]);

    //Get the indices from the model
    int  numIndices = meshNode->GetPolygonVertexCount();
    int* indices = meshNode->GetPolygonVertices();
    int  numVerts = meshNode->GetControlPointsCount();

    model->setVertexIndices(std::vector<int>(indices, indices + numIndices)); //Copy vector

    //Get the vertices from the model
    std::vector<Vector4> vertices;
    for (int j = 0; j < numVerts; j++) {
        FbxVector4 coord = meshNode->GetControlPointAt(j);
        vertices.push_back(Vector4((float)coord.mData[0], (float)coord.mData[1], (float)coord.mData[2], 1.0));
    }

    //Get the normals from the mesh
    std::vector<Vector4> normals;
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

    //Get the texture coordinates from the mesh
	FbxVector2 uv;
    std::vector<Texture2> textures;
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

    _loadTextures(model, meshNode, childNode);

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
        _buildTriangles(model, vertices, normals, textures, *model->getIndices(), translation, rotation, scale);
    }
}

void FbxLoader::_loadTextures(Model* model, FbxMesh* meshNode, FbxNode* childNode) {
    
    //Get material element info
    FbxLayerElementMaterial* pLayerMaterial = meshNode->GetLayer(0)->GetMaterials();   
    //Get material mapping info
	FbxLayerElementArrayTemplate<int> *tmpArray = &pLayerMaterial->GetIndexArray();   
	
    //No layers then no textures :(
	if(meshNode->GetLayerCount() == 0){
        return;
    }
	//Return the number of materials found in mesh
	int materialCount = childNode->GetSrcObjectCount<FbxSurfaceMaterial>();
	for (int materialIndex = 0; materialIndex < materialCount; ++materialIndex) {
		
        FbxSurfaceMaterial* material = (FbxSurfaceMaterial*)childNode->GetSrcObject<FbxSurfaceMaterial>(materialIndex);
		if (material == nullptr) {
            continue;
        }

		// This only gets the material of type sDiffuse,
        // you probably need to traverse all Standard Material Property by its name to get all possible textures.
		FbxProperty propDiffuse = material->FindProperty(FbxSurfaceMaterial::sDiffuse);

		// Get number of textures in the material which could contain diffuse, bump, normal and/or specular maps...
        int textureCount = propDiffuse.GetSrcObjectCount<FbxTexture>();

		for (int textureIndex = 0; textureIndex < textureCount; ++textureIndex) {
            //Fetch the diffuse texture
			FbxFileTexture* textureFbx = FbxCast<FbxFileTexture>(propDiffuse.GetSrcObject<FbxFileTexture>(textureIndex));
						
			// Then, you can get all the properties of the texture, including its name
			std::string textureName = textureFbx->GetFileName();
            std::string textureNameTemp = textureName; //Used a temporary storage to not overwrite textureName
            std::string texturePath = "..\\models\\textures";
            //Finds second to last position of string and use that for file access name
			texturePath.append(textureName.substr(textureNameTemp.substr(0, textureNameTemp.find_last_of("/\\")).find_last_of("/\\")));
            Texture* texture = new Texture(texturePath);
            model->addTexture(texture);
		}
	}
}

void FbxLoader::_buildTriangles(Model* model, std::vector<Vector4>& vertices, std::vector<Vector4>& normals,
    std::vector<Texture2>& textures, std::vector<int>& indices, Matrix translation, Matrix rotation, Matrix scale) {

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

