#include "Engine/Components/Core/Transform.h"
//#include "Engine/Components/Core/GameObject.h"

Transform::Transform() {

}

glm::mat4 Transform::GetTransform(glm::vec3 pos)
{
    //		glm::vec3 asd = glm::eulerToRadians(rotation);
    glm::mat4 rotation = glm::mat4(glm::quat(this->rotation));
    return glm::translate(glm::mat4(1.0f), pos)
        * rotation
        * glm::scale(glm::mat4(1.0f), scale * worldScale); // was scale * worldScale
}


void Transform::UpdateChildrenTransform(GameObject* gameObject) {
    if (gameObject->parent != nullptr) {
        gameObject->transform->worldScale = gameObject->transform->scale * gameObject->parent->transform->worldScale;
        gameObject->transform->worldPosition = (gameObject->transform->relativePosition * gameObject->parent->transform->worldScale + gameObject->parent->transform->worldPosition);
    }


    for (GameObject* child : gameObject->children) {
        child->transform->worldScale = child->transform->scale * child->parent->transform->worldScale;
        child->transform->worldPosition = (child->transform->relativePosition * child->parent->transform->worldScale + child->parent->transform->worldPosition);
        UpdateChildrenTransform(child);
    }
}



void Transform::UpdateChildrenTransform() {
    if (gameObject != nullptr) {
        UpdateChildrenTransform(gameObject);
    }
}


void Transform::UpdateChildrenScale(GameObject* gameObject) {
    /*
    if (gameObject->parent != nullptr) {
        gameObject->transform->worldScale = gameObject->transform->scale * gameObject->parent->transform->worldScale;
        gameObject->transform->worldPosition = (gameObject->transform->relativePosition + gameObject->parent->transform->worldPosition) * gameObject->transform->worldScale;
        gameObject->transform->relativePosition = gameObject->transform->relativePosition * gameObject->transform->worldScale;
    }


    for (GameObject* child : gameObject->children) {
        child->transform->worldScale = child->transform->scale * child->parent->transform->worldScale;
        gameObject->transform->worldPosition = (gameObject->transform->relativePosition + gameObject->parent->transform->worldPosition) * gameObject->transform->worldScale;
        gameObject->transform->relativePosition = gameObject->transform->relativePosition * gameObject->transform->worldScale;
        UpdateChildrenScale(child);
    }
    */
}

void Transform::UpdateChildrenScale() {
    if (gameObject != nullptr)
        UpdateChildrenScale(gameObject);
}
