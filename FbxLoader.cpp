#include "FbxLoader.h"
#include "AnimatedModel.h"
#include "Model.h"
#include <map>

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
        if (mesh != nullptr)
        {
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
			//Look for a skin animations
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
    int FPS = (int)frameTime.GetFrameRate(mode);
    int animationDuration = (((float)FPS*FPS) / (FRAMERATE))*((frameTime.GetMilliSeconds() / 1000.0f) / ((float)animationFrames));

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
		animation->addSkin(new SkinningData(pCluster, node, animationFrames)); //Add new skin data to frame
    }

	model->addAnimation(animation); //Push animation onto the animated model
}


void FbxLoader::loadModelData(Model* model, FbxMesh* meshNode, FbxNode* childNode) {

    ////Global transform
    FbxDouble3 Tt = childNode->EvaluateGlobalTransform().GetT();
    FbxDouble3 Rr = childNode->EvaluateGlobalTransform().GetR();
    FbxDouble3 Ss = childNode->EvaluateGlobalTransform().GetS();

    //Unpack to Vector3D form for processing
    FbxDouble* TBuff = Tt.Buffer();
    FbxDouble* RBuff = Rr.Buffer();
    FbxDouble* SBuff = Ss.Buffer();
	Vector4 T(TBuff[0], TBuff[1], TBuff[2], 1.0f);
	Vector4 R(RBuff[0], RBuff[1], RBuff[2], 1.0f);
	Matrix scale = Matrix::scale(SBuff[0], SBuff[1], SBuff[2]);

    //Compute rotation matrix from rotation Vector3D
	Matrix XRot = Matrix::rotationAroundX(R.getx());
	Matrix YRot = Matrix::rotationAroundY(R.gety());
	Matrix ZRot = Matrix::rotationAroundZ(R.getz());

    //Compute x, y and z rotation vectors by multiplying through
	Matrix rotation = XRot * YRot * ZRot;

    // ========= Get the indices from the mesh ===============
    int  numIndices = meshNode->GetPolygonVertexCount();
    int* indices = meshNode->GetPolygonVertices();
    int  numVerts = meshNode->GetControlPointsCount();

    // ========= Get the vertices from the mesh ==============
    std::vector<Vector4> vertices;
    for (int j = 0; j < numVerts; j++) {
        FbxVector4 coord = meshNode->GetControlPointAt(j);
        vertices.push_back(Vector4((float)coord.mData[0], (float)coord.mData[1], (float)coord.mData[2], 1.0));
    }

    // ========= Get the normals from the mesh ===============
    std::vector<Vector4> normals;
    std::map<int, Vector4> mappingNormals;
    //std::vector<Vector4> tempNormals;
    FbxGeometryElementNormal* normalElement = meshNode->GetElementNormal();
    const FbxLayerElement::EMappingMode mapMode = normalElement->GetMappingMode();
    if (normalElement) {
        if (mapMode == FbxLayerElement::EMappingMode::eByPolygonVertex) {
            int numNormals = meshNode->GetPolygonCount() * 3;

            // Loop through the triangle meshes
            for (int polyCounter = 0; polyCounter < numNormals; polyCounter++) {
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
            for (int lVertexIndex = 0; lVertexIndex < meshNode->GetControlPointsCount(); lVertexIndex++) {
                int lNormalIndex = 0;
                //reference mode is direct, the normal index is same as vertex index.
                //get normals by the index of control vertex
                if (normalElement->GetReferenceMode() == FbxGeometryElement::eDirect) {
                    lNormalIndex = lVertexIndex;
				}

                //reference mode is index-to-direct, get normals by the index-to-direct
				if (normalElement->GetReferenceMode() == FbxGeometryElement::eIndexToDirect) {
                    lNormalIndex = normalElement->GetIndexArray().GetAt(lVertexIndex);
				}

                //Got normals of each vertex.
                FbxVector4 normal = normalElement->GetDirectArray().GetAt(lNormalIndex);
                normals.push_back(Vector4((float)normal[0], (float)normal[1], (float)normal[2], 1.0));
			}
        }
    }

    int triCount = 0;
	int totalTriangles = numIndices / 3; //Each index represents one vertex and a triangle is 3 vertices
   
    //Read each triangle vertex indices and store them in triangle array
    for (int i = 0; i < totalTriangles; i++) {
		Vector4 A(vertices[indices[triCount]].getx(), 
									vertices[indices[triCount]].gety(), 
									vertices[indices[triCount]].getz(), 
									1.0);
		model->addVertex((rotation * scale * A) + T); //Scale then rotate vertex

		Vector4 AN(normals[indices[triCount]].getx(), 
									normals[indices[triCount]].gety(), 
									normals[indices[triCount]].getz(), 
									1.0);
		model->addNormal(rotation * AN); //Scale then rotate normal
		model->addDebugNormal((rotation * scale * A) + T);
		model->addDebugNormal(((rotation * scale * A) + T) + rotation * AN);
		triCount++;

		Vector4 B(vertices[indices[triCount]].getx(), 
									vertices[indices[triCount]].gety(), 
									vertices[indices[triCount]].getz(), 
									1.0);
		model->addVertex((rotation * scale * B) + T); //Scale then rotate vertex

		Vector4 BN(normals[indices[triCount]].getx(), 
									normals[indices[triCount]].gety(), 
									normals[indices[triCount]].getz(), 
									1.0);
		model->addNormal(rotation * BN); //Scale then rotate normal
		model->addDebugNormal((rotation * scale * B) + T);
		model->addDebugNormal(((rotation * scale * B) + T) + rotation * BN);
        triCount++;
            
		Vector4 C(vertices[indices[triCount]].getx(), 
									vertices[indices[triCount]].gety(), 
									vertices[indices[triCount]].getz(), 
									1.0);
		model->addVertex((rotation * scale * C) + T); //Scale then rotate vertex

		Vector4 CN(normals[indices[triCount]].getx(), 
									normals[indices[triCount]].gety(), 
									normals[indices[triCount]].getz(), 
									1.0);
		model->addNormal(rotation * CN); //Scale then rotate normal
		model->addDebugNormal((rotation * scale * C) + T);
		model->addDebugNormal(((rotation * scale * C) + T) + rotation * CN);
        triCount++;
    }
}
