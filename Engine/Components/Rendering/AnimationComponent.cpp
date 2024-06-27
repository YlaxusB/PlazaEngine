#include "Engine/Core/PreCompiledHeaders.h"
#include "AnimationComponent.h"
#include "ThirdParty/ufbx/ufbx/ufbx.h"

namespace Plaza {
	static inline glm::vec3 ConvertUfbxVec3(ufbx_vec3 vec) {
		return glm::vec3(vec.x, vec.y, vec.z);
	}
	static inline glm::quat ConvertUfbxQuat(ufbx_quat const& quaternionUfbx) {
		glm::vec4 vec4GLM;
		vec4GLM.x = quaternionUfbx.x;
		vec4GLM.y = quaternionUfbx.y;
		vec4GLM.z = quaternionUfbx.z;
		vec4GLM.w = quaternionUfbx.w;
		return glm::quat(vec4GLM);
	};
	static glm::mat4 ConvertUfbxMatrix(ufbx_matrix const& mat4Ufbx) {
		glm::mat4 mat4Glm;
		for (unsigned int column = 0; column < 4; ++column)
		{
			mat4Glm[0][column] = mat4Ufbx.cols[column].x;
			mat4Glm[1][column] = mat4Ufbx.cols[column].y;
			mat4Glm[2][column] = mat4Ufbx.cols[column].z;
			mat4Glm[3][column] = column < 3 ? 0.0f : 1.0f;
		}
		return mat4Glm;
	}
	void Animation::Play() {
		this->mIsPlaying = true;
		Application->activeScene->mPlayingAnimations.clear();
		Application->activeScene->mPlayingAnimations.emplace(this->mUuid, *this);
	}

	void AnimationComponent::GetAnimation(std::string filePath, std::map<uint64_t, Plaza::Bone>& bonesMap, unsigned int index) {
		this->mAnimations.clear();
		ufbx_load_opts opts = { };
		opts.target_axes = ufbx_axes_right_handed_y_up,
			opts.target_unit_meters = 1.0f;

		ufbx_error error;
		ufbx_scene* scene = ufbx_load_file(filePath.c_str(), &opts, &error);

		if (!scene) {
			fprintf(stderr, "Failed to load scene: %s\n", error.description.data);
			return;
		}
		int meshCount = scene->meshes.count;
		std::set<uint64_t> boneIds = std::set<uint64_t>();
		std::vector<ufbx_bone*> uniqueBones = std::vector<ufbx_bone*>();

		for (auto& [key, value] : VulkanRenderer::GetRenderer()->mBones) {
			value.mChildren.clear();
		}

		for (const ufbx_skin_deformer* skin : scene->skin_deformers) {
			for (const ufbx_skin_cluster* cluster : skin->clusters) {
				VulkanRenderer::GetRenderer()->mBones.at(cluster->bone_node->bone->element_id).mOffset = (ConvertUfbxMatrix(cluster->geometry_to_bone));
				if (cluster->bone_node->parent && cluster->bone_node->parent->bone) {
					VulkanRenderer::GetRenderer()->mBones.at(cluster->bone_node->bone->element_id).mParentId = cluster->bone_node->parent->bone->element_id;

					std::vector<uint64_t>& childrenVector = VulkanRenderer::GetRenderer()->mBones.at(cluster->bone_node->parent->bone->element_id).mChildren;

					if (std::find(childrenVector.begin(), childrenVector.end(), cluster->bone_node->bone->element_id) == childrenVector.end()) {
						childrenVector.push_back(cluster->bone_node->bone->element_id);
					}
				}
			}
		}


		for (const ufbx_anim_stack* stack : scene->anim_stacks)
		{
			ufbx_baked_anim* bake = ufbx_bake_anim(scene, stack->anim, nullptr, nullptr);
			assert(bake != nullptr);
			if (bake == nullptr)
				continue;
			Animation& animation = mAnimations.emplace_back(Animation());
			animation.mName = stack->name.data;

			animation.mRootParentTransform = ConvertUfbxMatrix(scene->root_node->geometry_to_world);
			//animation.mRootParentTransform = glm::mat4({
			//	animation.mRootParentTransform[0][0], 0.0f, 0.0f, 0.0f,
			//	0.0f, animation.mRootParentTransform[0][1], 0.0f, 0.0f,
			//	0.0f, 0.0f, animation.mRootParentTransform[0][2], 0.0f,
			//	0.0f, 0.0f, 0.0f, 1.0f }
			//);
			//animcomponent.start = (float)bake->playback_time_begin;
			//animcomponent.end = (float)bake->playback_time_end;
			for (const ufbx_baked_node& bake_node : bake->nodes)
			{
				ufbx_node* scene_node = scene->nodes[bake_node.typed_id];
				// Translation:

				std::map<float, glm::vec3> positions = std::map<float, glm::vec3>();
				for (const ufbx_baked_vec3& keyframe : bake_node.translation_keys)
				{
					positions[keyframe.time] = glm::vec3(keyframe.value.x, keyframe.value.y, keyframe.value.z);
					//animation.mTranslations[scene_node->bone->element_id].push_back(glm::vec3(keyframe.value.x, keyframe.value.y, keyframe.value.z));
					//animationdata.keyframe_times.push_back((float)keyframe.time);
					//animationdata.keyframe_data.push_back(keyframe.value.x);
					//animationdata.keyframe_data.push_back(keyframe.value.y);
					//animationdata.keyframe_data.push_back(keyframe.value.z);
				}
				
				

				// Rotation:
				std::map<float, glm::quat> rotations = std::map<float, glm::quat>();
				for (const ufbx_baked_quat& keyframe : bake_node.rotation_keys)
				{
					rotations[keyframe.time] = ConvertUfbxQuat(keyframe.value);
					//animation.mRotations[scene_node->bone->element_id].push_back(ConvertUfbxQuat(keyframe.value));
				}

				uint64_t boneUuid = scene_node->bone->element_id;
				unsigned int maxCount = glm::max(positions.size(), rotations.size());
				std::map<float, Bone::Keyframe> keyframes = std::map<float, Bone::Keyframe>();
				for (const auto& [key, value] : positions) {
					keyframes[key].position = value;
				}
				for (const auto& [key, value] : rotations) {
					keyframes[key].orientation = value;
				}
				for (const auto& [key, value] : keyframes) {
					animation.mKeyframes[boneUuid].push_back(Bone::Keyframe{ key, ConvertUfbxVec3(ufbx_evaluate_baked_vec3(bake_node.translation_keys, key)), ConvertUfbxQuat(ufbx_evaluate_baked_quat(bake_node.rotation_keys, key)), ConvertUfbxVec3(ufbx_evaluate_baked_vec3(bake_node.scale_keys, key)) });
				}
				if (animation.mKeyframes[boneUuid].size() == 0) {
					animation.mKeyframes[boneUuid].push_back(Bone::Keyframe{ 0.0f, glm::vec3(0.0f), glm::quat(glm::vec3(0.0f)), glm::vec3(1.0f) });
				}

				// Rotation:
				//{
				//	Entity animDataEntity = CreateEntity();
				//	AnimationDataComponent& animationdata = scene.animation_datas.Create(animDataEntity);
				//	scene.Component_Attach(animDataEntity, entity);
				//	for (const ufbx_baked_quat& keyframe : bake_node.rotation_keys)
				//	{
				//		animationdata.keyframe_times.push_back((float)keyframe.time);
				//		animationdata.keyframe_data.push_back(keyframe.value.x);
				//		animationdata.keyframe_data.push_back(keyframe.value.y);
				//		animationdata.keyframe_data.push_back(keyframe.value.z);
				//		animationdata.keyframe_data.push_back(keyframe.value.w);
				//	}
				//	AnimationComponent::AnimationChannel& channel = animcomponent.channels.emplace_back();
				//	channel.target = target;
				//	channel.path = AnimationComponent::AnimationChannel::Path::ROTATION;
				//	channel.samplerIndex = (int)animcomponent.samplers.size();
				//	AnimationComponent::AnimationSampler& sampler = animcomponent.samplers.emplace_back();
				//	sampler.mode = AnimationComponent::AnimationSampler::Mode::LINEAR;
				//	sampler.data = animDataEntity;
				//}
				//
				//// Scale:
				//{
				//	Entity animDataEntity = CreateEntity();
				//	AnimationDataComponent& animationdata = scene.animation_datas.Create(animDataEntity);
				//	scene.Component_Attach(animDataEntity, entity);
				//	for (const ufbx_baked_vec3& keyframe : bake_node.scale_keys)
				//	{
				//		animationdata.keyframe_times.push_back((float)keyframe.time);
				//		animationdata.keyframe_data.push_back(keyframe.value.x);
				//		animationdata.keyframe_data.push_back(keyframe.value.y);
				//		animationdata.keyframe_data.push_back(keyframe.value.z);
				//	}
				//	AnimationComponent::AnimationChannel& channel = animcomponent.channels.emplace_back();
				//	channel.target = target;
				//	channel.path = AnimationComponent::AnimationChannel::Path::SCALE;
				//	channel.samplerIndex = (int)animcomponent.samplers.size();
				//	AnimationComponent::AnimationSampler& sampler = animcomponent.samplers.emplace_back();
				//	sampler.mode = AnimationComponent::AnimationSampler::Mode::LINEAR;
				//	sampler.data = animDataEntity;
				//}

			}

			ufbx_free_baked_anim(bake);
		}

		ufbx_free_scene(scene);
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
	}
}