#pragma once
namespace Engine {
	class ModelComponent : public Component {
		shared_ptr<Model> model;
	};
}