#pragma once
#include "Engine/Application/EditorCamera.h"
#include "Engine/Components/Core/Entity.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/matrix_decompose.hpp>
#include <glm/trigonometric.hpp>
#include <glm/gtx/euler_angles.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>


#include <float.h>
#include <glm/gtx/euler_angles.hpp>

#include "Editor/GUI/TransformOverlay.h"
#include "Editor/GUI/guiMain.h"
#include "Engine/Application/ApplicationSizes.h"
#include "Editor/GUI/TransformOverlay.h"

using namespace Plaza;
using namespace Plaza::Editor;
namespace Plaza::Editor {
	class Gizmo {
	public:
		static void Draw(Entity* entity, Camera camera);

	private:

		/// <summary>
		/// Extract translation, rotation and scale from a Matrix 4 
		/// <para> Rotation is returned in radians </para>
		/// </summary>
		static bool DecomposeTransform(const glm::mat4& transform, glm::vec3& translation, glm::vec3& rotation, glm::vec3& scale);
	};
}