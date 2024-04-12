#include "Engine/Core/PreCompiledHeaders.h"
#include "Collider.h"
#include "Engine/Core/Physics.h"
#include "ThirdParty/PhysX/physx/include/geometry/PxHeightFieldGeometry.h"

Plaza::Vertex CombineVertices(const Plaza::Vertex& v1, const Plaza::Vertex& v2) {
	// Calculate the new position by averaging the positions of v1 and v2
	Plaza::Vertex combinedVertex = Plaza::Vertex(glm::vec3(0.0f));
	combinedVertex.position = (v1.position + v2.position) * 0.5f;
	// You might also want to consider other attributes like normals, UVs, etc.
	return combinedVertex;
}

float CalculateEdgeLength(const glm::vec3& p1, const glm::vec3& p2) {
	return glm::length(p2 - p1);
}

void SimplifyMesh(std::vector<Plaza::Vertex>& vertices, std::vector<unsigned int>& indices, int targetTriangleCount) {
	// Simplification loop
	while (indices.size() > targetTriangleCount * 3) {
		// Find the index of the triangle to remove
		float minCollapseCost = std::numeric_limits<float>::max();
		size_t collapseIndex = 0;

		for (size_t i = 0; i < indices.size(); i += 3) {
			// Calculate the cost of collapsing this triangle edge
			// You can use metrics like edge length, normal difference, etc.
			// Here, a simple metric is used: edge length
			float cost = CalculateEdgeLength(vertices[indices[i]].position, vertices[indices[i + 1]].position);

			if (cost < minCollapseCost) {
				minCollapseCost = cost;
				collapseIndex = i;
			}
		}

		// Collapse the edge
		vertices[indices[collapseIndex]] = CombineVertices(vertices[indices[collapseIndex]],
			vertices[indices[collapseIndex + 1]]);
		vertices[indices[collapseIndex + 1]].isValid = false; // Mark vertex as invalid

		// Update indices to remove the collapsed triangle
		indices.erase(indices.begin() + collapseIndex, indices.begin() + collapseIndex + 3);
	}

	// Remove invalid vertices
	vertices.erase(std::remove_if(vertices.begin(), vertices.end(),
		[](const Plaza::Vertex& vertex) { return !vertex.isValid; }),
		vertices.end());
}

namespace Plaza {

	Collider::Collider(std::uint64_t uuid, RigidBody* rigidBody) {
		this->uuid = uuid;
		if (Application->runningScene)
			this->Init(rigidBody);
	}

	Collider::~Collider() {
		if (mRigidActor) {
			for (ColliderShape* shape : mShapes) {
				this->mRigidActor->detachShape(*shape->mPxShape);
				if (shape->mPxShape->userData)
					shape->mPxShape->release();
			}
			Physics::m_scene->removeActor(*this->mRigidActor);
			if (this->mRigidActor->userData)
				this->mRigidActor->release();
			if (this->material->userData)
				this->material->release();
		}
	}

	void Collider::RemoveActor() {
		if (this->mRigidActor) {
			for (ColliderShape* shape : mShapes) {
				this->mRigidActor->detachShape(*shape->mPxShape);
			}
			Physics::m_scene->removeActor(*this->mRigidActor);
			this->mRigidActor->release();
			mRigidActor = nullptr;
		}
	}

	void Collider::Init(RigidBody* rigidBody) {
		// Check if Rigid Body exists
		this->mDynamic = rigidBody != nullptr;
		this->lastScale = glm::vec3(0.0f);
		if (rigidBody)
			this->mDynamic = !rigidBody->kinematic;
		if (Application->runningScene || Application->copyingScene) {
			InitCollider(rigidBody);
		}
	}

	void Collider::RemoveCollider() {
		this->RemoveActor();
	}

	void Collider::InitCollider(RigidBody* rigidBody) {
		//this->RemoveActor();
		physx::PxTransform pxTransform = Physics::GetPxTransform(Application->activeScene->transformComponents.at(this->uuid));
		this->mRigidActor = Physics::m_physics->createRigidDynamic(pxTransform);
		if (this->mRigidActor == nullptr)
			this->mRigidActor = Physics::m_physics->createRigidDynamic(physx::PxTransform(physx::PxIdentity(1.0f)));
		if (!material) {
			material = Physics::defaultMaterial;
			if (this->mDynamic) {
				material = Physics::m_physics->createMaterial(rigidBody->mStaticFriction, rigidBody->mDynamicFriction, rigidBody->mRestitution);
			}
		}
		if (!mDynamic)
			this->mRigidActor->is<physx::PxRigidDynamic>()->setRigidBodyFlag(physx::PxRigidBodyFlag::eKINEMATIC, true);


		this->mRigidActor->userData = reinterpret_cast<void*>(this->uuid);
		// Attach the shapes with the material to the actor
		for (ColliderShape* shape : mShapes) {
			//shape->mPxShape->setMaterials(&material, 1);
			//physx::PxFilterData filterData;
			//filterData.word0 = 0; // word0 = own ID
			//filterData.word1 = 0;  // word1 = ID mask to filter pairs that trigger a contact callback
			//shape->mPxShape->setSimulationFilterData(filterData);
			shape->mPxShape->userData = reinterpret_cast<void*>(this->uuid);
			this->mRigidActor->attachShape(*shape->mPxShape);
		}
		Physics::m_scene->addActor(*this->mRigidActor);
		//this->UpdateShapeScale(Application->activeScene->transformComponents.at(this->uuid).GetWorldScale());
	}

	void Collider::AddConvexMeshShape(Mesh* mesh) {
		physx::PxShape* shape;
		physx::PxTriangleMeshGeometry triangleGeometry;
		std::vector<physx::PxVec3> vertices;
		for (const glm::vec3& vertex : mesh->vertices) {
			vertices.push_back(physx::PxVec3(vertex.x, vertex.y, vertex.z));
		}
		physx::PxConvexMeshDesc meshDesc;
		meshDesc.points.data = vertices.data();
		meshDesc.points.count = static_cast<physx::PxU32>(vertices.size());
		meshDesc.points.stride = sizeof(physx::PxVec3);
		meshDesc.flags = physx::PxConvexFlag::eCOMPUTE_CONVEX;

		physx::PxTolerancesScale scale;
		physx::PxCookingParams params(scale);

		physx::PxDefaultMemoryOutputStream buf;
		physx::PxConvexMeshCookingResult::Enum result;
		if (!PxCookConvexMesh(params, meshDesc, buf, &result))
			printf("failed");
		physx::PxDefaultMemoryInputData input(buf.getData(), buf.getSize());
		physx::PxConvexMesh* convexMesh = Physics::m_physics->createConvexMesh(input);
		shape = Physics::m_physics->createShape(physx::PxConvexMeshGeometry(convexMesh),
			*Physics::defaultMaterial);

		this->mShapes.push_back(new ColliderShape(shape, ColliderShape::ColliderShapeEnum::CONVEX_MESH, mesh->meshId));
		delete mesh;
	}

	void Collider::AddMeshShape(Mesh* mesh) {
		//SimplifyMesh(mesh->vertices, mesh->indices, 50);
		auto start0 = std::chrono::high_resolution_clock::now();
		physx::PxShape* shape;
		physx::PxTriangleMeshGeometry triangleGeometry;
		std::vector<physx::PxVec3> vertices = std::vector<physx::PxVec3>();
		std::vector<physx::PxU32> indices = std::vector<physx::PxU32>();

		// Extract vertices and indices from the mesh
		auto start1 = std::chrono::high_resolution_clock::now();
		vertices.resize(mesh->vertices.size());
		std::transform(mesh->vertices.begin(), mesh->vertices.end(), vertices.begin(),
			[](const glm::vec3& glmVertex) {
				return physx::PxVec3(glmVertex.x, glmVertex.y, glmVertex.z);
			}
		);
		indices.resize(mesh->indices.size());
		indices = mesh->indices;

		auto end1 = std::chrono::high_resolution_clock::now();
		//for (const glm::vec3& vertex : mesh->vertices) {
		//	vertices.push_back(physx::PxVec3(vertex.x, vertex.y, vertex.z));
		//}
		//for (const unsigned int& index : mesh->indices) {
		//	indices.push_back(static_cast<physx::PxU32>(index));
		//}

		auto start2 = std::chrono::high_resolution_clock::now();
		physx::PxTriangleMeshDesc meshDesc;

		meshDesc.points.data = vertices.data();
		meshDesc.points.count = static_cast<physx::PxU32>(vertices.size());
		meshDesc.points.stride = sizeof(physx::PxVec3);
		meshDesc.triangles.data = indices.data();
		meshDesc.triangles.count = static_cast<physx::PxU32>(indices.size()) / 3;
		meshDesc.triangles.stride = sizeof(physx::PxU32) * 3;
		//meshDesc. = physx::PxSDFDesc::
		physx::PxCookingParams params(Physics::m_physics->getTolerancesScale());
		params.meshPreprocessParams |= physx::PxMeshPreprocessingFlag::eDISABLE_CLEAN_MESH;
		// disable edge precompute, edges are set for each triangle, slows contact generation
		params.meshPreprocessParams |= physx::PxMeshPreprocessingFlag::eDISABLE_ACTIVE_EDGES_PRECOMPUTE;
		// lower hierarchy for internal mesh
		//params.meshcook = physx::PxMeshCookingHint::eCOOKING_PERFORMANCE;

		//params. = physx::PxMeshCookingHint::eSIM_PERFORMANCE;
		auto end2 = std::chrono::high_resolution_clock::now();
		auto start3 = std::chrono::high_resolution_clock::now();
		physx::PxDefaultMemoryOutputStream buf;
		if (!PxCookTriangleMesh(params, meshDesc, buf))
			printf("failed");
		auto end3 = std::chrono::high_resolution_clock::now();
		auto start4 = std::chrono::high_resolution_clock::now();
		physx::PxDefaultMemoryInputData input(buf.getData(), buf.getSize());
		physx::PxTriangleMesh* triangleMesh = Physics::m_physics->createTriangleMesh(input);
		shape = Physics::m_physics->createShape(physx::PxTriangleMeshGeometry(triangleMesh),
			*Physics::defaultMaterial);
		auto end4 = std::chrono::high_resolution_clock::now();
		this->mShapes.push_back(new ColliderShape(shape, ColliderShape::ColliderShapeEnum::MESH, mesh->meshId));
		auto end0 = std::chrono::high_resolution_clock::now();

		std::cout << "Time taken by function 1: " << std::chrono::duration_cast<std::chrono::milliseconds>(end1 - start1).count() << " milliseconds" << std::endl;
		std::cout << "Time taken by function 2: " << std::chrono::duration_cast<std::chrono::milliseconds>(end2 - start2).count() << " milliseconds" << std::endl;
		std::cout << "Time taken by function 3: " << std::chrono::duration_cast<std::chrono::milliseconds>(end3 - start3).count() << " milliseconds" << std::endl;
		std::cout << "Time taken by function 4: " << std::chrono::duration_cast<std::chrono::milliseconds>(end4 - start4).count() << " milliseconds" << std::endl;
		std::cout << "Time taken by function 0: " << std::chrono::duration_cast<std::chrono::milliseconds>(end0 - start0).count() << " milliseconds" << std::endl;
		//delete mesh;
	}

	void Collider::AddHeightShape(float** heightData, int size) {
		auto start0 = std::chrono::high_resolution_clock::now();
		physx::PxShape* shape;
		physx::PxHeightFieldSample* hfSamples = new physx::PxHeightFieldSample[size * size];

		physx::PxReal min = PX_MAX_F32;
		physx::PxReal max = -PX_MAX_F32;

		OpenGLMesh* debugMesh = new OpenGLMesh();
		debugMesh->vertices.reserve(size);
		debugMesh->indices.reserve(size);
		debugMesh->indices.push_back(0);
		debugMesh->indices.push_back(0);
		debugMesh->indices.push_back(0);
		physx::PxReal quantization = (physx::PxReal)0x7fff;

		for (unsigned int col = 0; col < size; col++)
		{
			for (unsigned int row = 0; row < size; row++)
			{
				min = physx::PxMin(min, heightData[col][row]);
				max = physx::PxMax(max, heightData[col][row]);
			}
		}
		using namespace physx;
		physx::PxReal deltaHeight = max - min;
		physx::PxReal heightScale = physx::PxMax<physx::PxReal>(deltaHeight / quantization, PX_MIN_HEIGHTFIELD_Y_SCALE);

		for (unsigned int col = 0; col < size; col++)
		{
			for (unsigned int row = 0; row < size; row++)
			{
				physx::PxI16 height = physx::PxI16(quantization * ((heightData[col][row] - min) / deltaHeight));
				physx::PxHeightFieldSample& smp = hfSamples[(row * size) + col];  // Corrected indexing here
				smp.height = heightData[col][row];
			}
		}



		// Cook the height field
		physx::PxHeightFieldDesc heightFieldDesc;
		heightFieldDesc.format = physx::PxHeightFieldFormat::eS16_TM;
		heightFieldDesc.nbColumns = size;
		heightFieldDesc.nbRows = size;
		heightFieldDesc.samples.data = hfSamples;
		heightFieldDesc.flags = physx::PxHeightFieldFlags();
		heightFieldDesc.samples.data = new float[size * size];
		heightFieldDesc.samples.stride = sizeof(float);
		char* currentByte = (char*)heightFieldDesc.samples.data;
		for (physx::PxU32 row = 0; row < size; row++)
		{
			for (physx::PxU32 col = 0; col < size; col++)
			{
				physx::PxHeightFieldSample* currentSample = (physx::PxHeightFieldSample*)currentByte;
				currentSample->height = (float)heightData[row][col];
				currentByte += heightFieldDesc.samples.stride;
			}
		}

		physx::PxHeightField* aHeightField = PxCreateHeightField(heightFieldDesc);
		std::cout << "First: " << heightData[255][255] << "\n";
		std::cout << "Second: " << aHeightField->getSample(255, 255).height << "\n";
		std::cout << "Third: " << size * 0.5f * 20 << "\n";
		physx::PxHeightFieldGeometry hfGeom(aHeightField, physx::PxMeshGeometryFlags(), 1, 20,
			20);


		float terrainWidth = 256 / 2 * 20;
		PxTransform localPose;
		localPose.p = PxVec3(-terrainWidth,    // make it so that the center of the
			0, -terrainWidth);         // heightfield is at world (0,minHeight,0)
		localPose.q = PxQuat(PxIdentity);
		//shape->setLocalPose(localPose);
		shape = Physics::m_physics->createShape(hfGeom,
			*Physics::defaultMaterial, false);
		shape->setLocalPose(localPose);
		this->mShapes.push_back(new ColliderShape(shape, ColliderShape::ColliderShapeEnum::HEIGHT_FIELD));
		auto end0 = std::chrono::high_resolution_clock::now();
		delete[] hfSamples;

		std::cout << "Time taken by function 0: " << std::chrono::duration_cast<std::chrono::milliseconds>(end0 - start0).count() << " milliseconds" << std::endl;
		//delete mesh;
	}

	void Collider::CreateShape(ColliderShape::ColliderShapeEnum shapeEnum, Transform* transform, Mesh* mesh) {
		if (shapeEnum == ColliderShape::ColliderShapeEnum::BOX) {
			physx::PxBoxGeometry geometry(transform->scale.x / 2.1, transform->scale.y / 2.1, transform->scale.z / 2.1);
			this->AddShape(new ColliderShape(Physics::m_physics->createShape(geometry, *Physics::defaultMaterial), ColliderShape::ColliderShapeEnum::BOX, 0));
		}
		else if (shapeEnum == ColliderShape::ColliderShapeEnum::SPHERE) {
			physx::PxSphereGeometry geometry(1.0f);
			this->AddShape(new ColliderShape(Physics::m_physics->createShape(geometry, *Physics::defaultMaterial), ColliderShape::ColliderShapeEnum::SPHERE, 0));
		}
		else if (shapeEnum == ColliderShape::ColliderShapeEnum::PLANE) {
			physx::PxBoxGeometry geometry(transform->scale.x / 2.1, 0.001f, transform->scale.z / 2.1);
			this->AddShape(new ColliderShape(Physics::m_physics->createShape(geometry, *Physics::defaultMaterial), ColliderShape::ColliderShapeEnum::PLANE, 0));
		}
		else if (shapeEnum == ColliderShape::ColliderShapeEnum::CAPSULE) {
			/* TODO: Implement capsule shape*/
			std::cout << "Capsule not yet implemented" << std::endl;
		}
		else if (shapeEnum == ColliderShape::ColliderShapeEnum::CYLINDER) {
			/* TODO: Implement cylinder shape*/
			std::cout << "Cylinder not yet implemented" << std::endl;
		}
		else if (shapeEnum == ColliderShape::ColliderShapeEnum::MESH) {
			this->AddMeshShape(mesh);
		}
		else if (shapeEnum == ColliderShape::ColliderShapeEnum::CONVEX_MESH) {
			this->AddConvexMeshShape(mesh);
		}
	}

	void Collider::AddShape(ColliderShape* shape) {
		this->mShapes.push_back(shape);
		Init(nullptr);
	}

	void Collider::UpdateAllShapesScale() {
		UpdateShapeScale(Application->activeScene->transformComponents.at(this->uuid).GetWorldScale());
	}

	void Collider::UpdateShapeScale(glm::vec3 scale) {
		PLAZA_PROFILE_SECTION("Collider: Update Shape Scale");
		if (lastScale != scale) {
			if (scale.x == 0 || scale.y == 0 || scale.z == 0) {
				lastScale = scale;
				return;
			}

			for (int i = 0; i < this->mShapes.size(); ++i) {
				physx::PxShape* shape = this->mShapes[i]->mPxShape;
				physx::PxGeometryHolder geometry = this->mShapes[i]->mPxShape->getGeometry();
				physx::PxShape* newShape = this->mShapes[i]->mPxShape;
				physx::PxMaterial* material;
				this->mShapes[i]->mPxShape->getMaterials(&material, 1);
				// Scale the geometry parameters by the given factor
				if (this->mShapes[i]->mEnum == ColliderShape::ColliderShapeEnum::BOX) {
					physx::PxBoxGeometry boxGeom = geometry.box();
					boxGeom.halfExtents = physx::PxVec3(scale.x / 2, scale.y / 2, scale.z / 2);
					shape->release();
					newShape = Physics::m_physics->createShape(boxGeom, *material);
				}
				else if (this->mShapes[i]->mEnum == ColliderShape::ColliderShapeEnum::PLANE) {
					physx::PxBoxGeometry planeGeom = geometry.box();
					planeGeom.halfExtents = physx::PxVec3(scale.x / 2, 0.001f, scale.z / 2);
					shape->release();
					newShape = Physics::m_physics->createShape(planeGeom, *material);
				}
				else if (this->mShapes[i]->mEnum == ColliderShape::ColliderShapeEnum::SPHERE) {
					physx::PxSphereGeometry sphereGeometry = geometry.sphere();
					//boxGeom.halfExtents = physx::PxVec3(scale.x / 2, scale.y / 2, scale.z / 2);
					sphereGeometry.radius = (scale.x + scale.y + scale.z) / 3;
					shape->release();
					newShape = Physics::m_physics->createShape(sphereGeometry, *material);
					//physx::PxSphereGeometry sphereGeom;
					//sphereGeom.radius *= 3;
					//this->mShapes[i]->setGeometry(physx::PxSphereGeometry(sphereGeom));
				}
				else if (this->mShapes[i]->mEnum == ColliderShape::ColliderShapeEnum::MESH) {
					physx::PxTriangleMeshGeometry meshGeometry(geometry.triangleMesh().triangleMesh, physx::PxMeshScale(physx::PxVec3(scale.x, scale.y, scale.z), physx::PxQuat(physx::PxIdentity)));
					//meshGeometry.scale = physx::PxMeshScale(physx::PxVec3(scale.x, scale.y, scale.z));
					newShape = Physics::m_physics->createShape(meshGeometry, *material);
					shape->release();
					//physx::PxSphereGeometry sphereGeom;
					//sphereGeom.radius *= 3;
					//this->mShapes[i]->setGeometry(physx::PxSphereGeometry(sphereGeom));
				}
				else if (this->mShapes[i]->mEnum == ColliderShape::ColliderShapeEnum::HEIGHT_FIELD) {
					physx::PxHeightFieldGeometry heightGeometry = geometry.heightField();
					heightGeometry.heightScale = 1.0f;
					shape->release();
					newShape = Physics::m_physics->createShape(heightGeometry, *material);
					//physx::PxSphereGeometry sphereGeom;
					//sphereGeom.radius *= 3;
					//this->mShapes[i]->setGeometry(physx::PxSphereGeometry(sphereGeom));
				}
				else {
					std::cout << "Shape not supported for scaling" << std::endl;
				}


				if (Application->runningScene && this->mRigidActor) {
					if (this->mDynamic) {
						this->mRigidActor->detachShape(*this->mShapes[i]->mPxShape);
						this->mShapes[i]->mPxShape = newShape;
						this->mRigidActor->attachShape(*newShape);
					}
					else {
						this->mRigidActor->detachShape(*this->mShapes[i]->mPxShape);
						this->mShapes[i]->mPxShape = newShape;
						this->mRigidActor->attachShape(*newShape);
					}
				}
			}
		}
		lastScale = scale;

	}

	template <typename EnumType>
	void Collider::SetSelfAndChildrenFlag(EnumType flag, bool value) {
		SetFlag(&Application->activeScene->rigidBodyComponents.at(this->uuid).mRigidActor, flag, value);
		for (uint64_t child : Application->activeScene->entities.at(this->uuid).childrenUuid) {
			SetFlag(&Application->activeScene->rigidBodyComponents.at(child).mRigidActor, flag, value);
		}
	}

	void Collider::SetFlag(physx::PxRigidActor* actor, physx::PxActorFlag::Enum flag, bool value) {
		actor->setActorFlag(flag, value);
	}
	void Collider::SetFlag(physx::PxRigidBody* rigidBody, physx::PxRigidBodyFlag::Enum flag, bool value) {
		rigidBody->setRigidBodyFlag(flag, value);
	}
	void Collider::SetFlag(Collider* collider, physx::PxRigidBodyFlag::Enum flag, bool value) {
		collider->mStaticPxRigidBody->setRigidBodyFlag(flag, value);
	}
	void Collider::SetFlags(Collider* collider, physx::PxRigidDynamicLockFlags flags) {
		collider->mRigidActor->is<physx::PxRigidDynamic>()->setRigidDynamicLockFlags(flags);
	}

	glm::quat GetQuaternionFromMatrix(const glm::mat4& matrix) {
		glm::mat3 rotationMatrix = glm::mat3(matrix); // Extract rotation component
		glm::quat rotationQuaternion = glm::quat(rotationMatrix); // Convert rotation matrix to quaternion
		return rotationQuaternion;
	}

	void Collider::UpdatePose(Transform* transform) {
		PLAZA_PROFILE_SECTION("Collider: Update Pose");
		if (transform) {
			if (this->mRigidActor) {
				glm::quat quaternion = glm::quat(transform->GetWorldRotation());

				physx::PxQuat pxQuaternion(quaternion.x, quaternion.y, quaternion.z, quaternion.w);

				glm::vec3 transformPos = transform->GetWorldPosition();
				physx::PxVec3 pxTranslation(transformPos.x, transformPos.y, transformPos.z);
				physx::PxTransform pxTransform(pxTranslation, pxQuaternion);
				//pxTransform = pxTransform.getNormalized();

				this->mRigidActor->setGlobalPose(pxTransform);
			}
		}
	}

	void Collider::UpdatePose() {
		this->UpdatePose(this->GetGameObject()->GetComponent<Transform>());
	}
}