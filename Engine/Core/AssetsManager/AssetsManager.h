#pragma once

namespace Plaza {
	class AssetsModelListStructure : public std::vector<Model*> {

	};



	class AssetsManager {
	public:
		void AddModel(Model* model) {
			this->mMemoryModels.push_back(model);
		}
		AssetsModelListStructure mMemoryModels = AssetsModelListStructure();
	};
}