#pragma once
namespace Plaza {
	class ModelComponent : public Component {
		shared_ptr<Model> model;
	};
}