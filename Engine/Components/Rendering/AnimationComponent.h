#pragma once

namespace Plaza {
	class Animation {
	public:
		uint64_t mUuid = 0;
		std::string mName = "";
		glm::mat4 mRootParentTransform = glm::mat4(1.0f);
		std::map<uint64_t, std::vector<float>> mTimes = std::map<uint64_t, std::vector<float>>();
		std::map<uint64_t, std::vector<glm::vec3>> mTranslations = std::map<uint64_t, std::vector<glm::vec3 >>();
		std::map<uint64_t, std::vector<glm::quat>> mRotations = std::map<uint64_t, std::vector<glm::quat>>();
		std::map<uint64_t, std::vector<Bone::Keyframe>> mKeyframes = std::map<uint64_t, std::vector<Bone::Keyframe>>();

		void Play();
		void Stop() {};
		bool GetState() { return mIsPlaying; };

	private:
		bool mIsPlaying = false;
		std::vector<int64_t> mBonesIds = std::vector<int64_t>();
	};

	class AnimationComponent : public Component {
	public:
		float mImportScale = 1.0f;//0.01f;
		std::vector<Animation> mAnimations = std::vector<Animation>();
		void GetAnimation(std::string filePath, std::map<uint64_t, Plaza::Bone>& bonesMap, unsigned int index = 0); //{
			//std::ofstream outfile;
			//FILE* fp = fopen(filePath.c_str(), "rb");
			//if (!fp) {
			//	std::cout << "Failed to open: " << filePath << "\n";
			//	return;
			//}
			//fseek(fp, 0, SEEK_END);
			//long file_size = ftell(fp);
			//fseek(fp, 0, SEEK_SET);
			//auto* content = new ofbx::u8[file_size];
			//fread(content, 1, file_size, fp);
			//ofbx::IScene* scene = ofbx::load((ofbx::u8*)content, file_size, (ofbx::u64)ofbx::LoadFlags::TRIANGULATE);
			//delete[] content;
			//fclose(fp);
			//
			//int meshCount = scene->getMeshCount();
			//std::set<uint64_t> boneIds = std::set<uint64_t>();
			//std::vector<const ofbx::Object*> uniqueBones = std::vector<const ofbx::Object*>();
			//
			//for (int meshIndex = 0; meshIndex < meshCount; ++meshIndex) {
			//	const ofbx::Mesh& mesh = *scene->getMesh(meshIndex);
			//	const ofbx::Skin* skin = mesh.getGeometry()->getSkin();
			//	if (skin) {
			//		const int clusterCount = skin->getClusterCount();
			//		for (int j = 0; j < clusterCount; ++j) {
			//			const ofbx::Cluster* cluster = skin->getCluster(j);
			//			if (cluster->getIndicesCount() <= 0)
			//				continue;
			//			const int* indices = cluster->getIndices();
			//			const double* weightsData = cluster->getWeights();
			//
			//			if (boneIds.find(cluster->getLink()->id) == boneIds.end())
			//				uniqueBones.push_back(cluster->getLink());
			//
			//			boneIds.emplace(cluster->getLink()->id);
			//
			//			bonesMap[cluster->getLink()->id].mOffset = glm::inverse(ofbxToGlm(cluster->getTransformLinkMatrix(), glm::vec3(1.0f)));
			//			//finalMesh->uniqueBonesInfo.emplace(cluster->getLink()->id, Bone { cluster->getLink()->id, cluster->getLink()->getParent()->id, cluster->name });
			//		}
			//	}
			//}
			//for (int i = 0; i < scene->getAnimationStackCount(); ++i) {
			//	Animation& animation = mAnimations.emplace_back(Animation());
			//
			//	const ofbx::AnimationStack* animationStack = (ofbx::AnimationStack*)scene->getAnimationStack(i);
			//	const ofbx::TakeInfo* takeInfo = scene->getTakeInfo(animationStack->name);
			//	if (takeInfo) {
			//		if (takeInfo->name.begin != takeInfo->name.end) {
			//			animation.mName = DataViewToString(takeInfo->name);
			//		}
			//		if (animation.mName.empty() && takeInfo->filename.begin != takeInfo->filename.end) {
			//			animation.mName = DataViewToString(takeInfo->filename);
			//		}
			//		if (animation.mName.empty()) animation.mName = "anim";
			//
			//	}
			//	else {
			//		animation.mName = "";
			//	}
			//
			//	/*
			//	const ofbx::AnimationStack* stack = loadedScene->getAnimationStack(i);
			//	for (int j = 0; stack->getLayer(j); ++j) {
			//		const ofbx::AnimationLayer* layer = stack->getLayer(j);
			//		for (int k = 0; layer->getCurveNode(k); ++k) {
			//			const ofbx::AnimationCurveNode* node = layer->getCurveNode(k);
			//			if (node->getBone())
			//			{
			//				const ofbx::Object* bone = node->getBone();
			//				uint64_t parentId = bone->getParent()->id;
			//
			//				Plaza::Bone plazaBone{ bone->id, bone->name };
			//				if (finalMesh->uniqueBonesInfo.find(parentId) != finalMesh->uniqueBonesInfo.end()) {
			//					finalMesh->uniqueBonesInfo[parentId].mChildren.push_back(bone->id);
			//					finalMesh->uniqueBonesInfo[bone->id].mOffset = finalMesh->uniqueBonesInfo[parentId].mOffset * finalMesh->uniqueBonesInfo[bone->id].mOffset;
			//				}
			//			};
			//		}
			//	}
			//	*/
			//
			//	for (int i = 0; animationStack->getLayer(i); ++i) {
			//		const ofbx::AnimationLayer* layer = animationStack->getLayer(i);
			//
			//		//if (!layer || !layer->getCurveNode(0)) {
			//		//	mAnimations.pop_back();
			//		//	continue;
			//		//}
			//		std::map<uint64_t, glm::vec3> translations = std::map<uint64_t, glm::vec3>();
			//		std::map<uint64_t, glm::quat>  rotations = std::map<uint64_t, glm::quat>();
			//
			//		for (const ofbx::Object* bone : uniqueBones) {
			//			//for (int k = 0; layer->getCurveNode(k); ++k) {
			//			const ofbx::AnimationCurveNode* translationNode = layer->getCurveNode(*bone, "Lcl Translation");
			//			const ofbx::AnimationCurveNode* rotationNode = layer->getCurveNode(*bone, "Lcl Rotation");
			//
			//			size_t vectorsSize = 0;
			//			if (translationNode) {
			//				vectorsSize = translationNode->getCurve(0)->getKeyCount();
			//				for (size_t i = 0; i < vectorsSize; ++i) {
			//					animation.mTimes[bone->id].push_back(ofbx::fbxTimeToSeconds(translationNode->getCurve(0)->getKeyTime()[i]));
			//				}
			//			}
			//			else if (rotationNode) {
			//				vectorsSize = rotationNode->getCurve(0)->getKeyCount();
			//				for (size_t i = 0; i < vectorsSize; ++i) {
			//					animation.mTimes[bone->id].push_back(ofbx::fbxTimeToSeconds(rotationNode->getCurve(0)->getKeyTime()[i]));
			//				}
			//			}
			//
			//
			//
			//			if (translationNode) {
			//				for (size_t i = 0; i < vectorsSize; ++i) {
			//					animation.mTranslations[bone->id].push_back(glm::vec3(translationNode->getCurve(0)->getKeyValue()[i] * mImportScale, translationNode->getCurve(1)->getKeyValue()[i] * mImportScale, translationNode->getCurve(2)->getKeyValue()[i] * mImportScale));
			//				}
			//			}
			//
			//			if (rotationNode) {
			//				for (size_t i = 0; i < vectorsSize; ++i) {
			//					glm::vec3 rotation = glm::vec3(rotationNode->getCurve(0)->getKeyValue()[i], rotationNode->getCurve(1)->getKeyValue()[i], rotationNode->getCurve(2)->getKeyValue()[i]);
			//					animation.mRotations[bone->id].push_back(rotation);
			//				}
			//			}
			//
			//
			//		}
			//		//}
			//	}
			//	//anim_layer->getCurveNode(bone)
			//
			//
			//		//bool data_found = false;
			//		//for (int k = 0; anim_layer->getCurveNode(k); ++k) {
			//		//	const ofbx::AnimationCurveNode* node = anim_layer->getCurveNode(k);
			//		//	//node->
			//		//	////if (node->getBone() == "Lcl Translation" || node->getBoneLinkProperty() == "Lcl Rotation") {
			//		//	//	if (!isConstCurve(node->getCurve(0)) || !isConstCurve(node->getCurve(1)) || !isConstCurve(node->getCurve(2))) {
			//		//	//		data_found = true;
			//		//	//		break;
			//		//	//	}
			//		//	//}
			//		//}
			//}
			//
			//for (Animation& animation : mAnimations) {
			//	std::cout << "Animation found: " << animation.mName << "\n";
			//}
		//}

	private:
		friend class Animation;
		//std::string DataViewToString(ofbx::DataView data) {
		//	char out[128];
		//	data.toString(out);
		//	std::string result(out);
		//	return result;
		//}
		//
		//static bool isConstCurve(const ofbx::AnimationCurve* curve) {
		//	if (!curve) return true;
		//	if (curve->getKeyCount() <= 1) return true;
		//	const float* values = curve->getKeyValue();
		//	if (curve->getKeyCount() == 2 && fabsf(values[1] - values[0]) < 1e-6) return true;
		//	return false;
		//}
		//
		//static glm::mat4 ofbxToGlm(const ofbx::Matrix& ofbxMat, glm::vec3 scale) {
		//	glm::mat4 glmMat = glm::mat4(1.0f);
		//
		//	for (int row = 0; row < 4; ++row) {
		//		for (int col = 0; col < 4; ++col) {
		//			double v = ofbxMat.m[4 * col + row];
		//			glmMat[col][row] = v;
		//		}
		//	}
		//
		//	return glmMat;
		//}
	};
}