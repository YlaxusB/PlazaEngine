#pragma once
#include <vector>
#include <variant>
#include <unordered_map>
#include "Engine/Components/Component.h"
#include "Engine/Core/Engine.h"

namespace Plaza {
	template <typename Key, typename T>
	class ComponentMultiMap : public std::unordered_multimap<Key, T> {
	public:
		std::function<T* ()> NewInstanceFactory;
		std::function<T* (T*)> CopyInstanceFactory;
		std::function<T* (uint64_t, T&)> EmplaceFactory;

		ComponentMultiMap() {
			NewInstanceFactory = []() -> T* {
				return new T();
				};
			CopyInstanceFactory = [](T* instanceToCopy) -> T* {
				return new T(*instanceToCopy);
				};
			EmplaceFactory = [this](uint64_t uuid, T& obj) -> T* {
				auto result = this->emplace(uuid, obj);
				return nullptr;//&result.first->second;
				};
		}

		T& at(const Key& key) {
			auto range = this->equal_range(key);
			if (range.first != range.second) {
				return range.first->second; // Returning the first value in the range
			}
		}

		T& operator [](const Key& key) {
			auto range = this->equal_range(key);
			if (range.first != range.second) {
				return range.first->second; // Returning the first value in the range
			}
		}

		T* GetComponent(uint64_t uuid) {
			T* component = nullptr;
			std::string className = typeid(T).name();
			auto it = this->find(uuid);
			if (it != this->end()) {
				component = &(it->second);
			}
			return static_cast<T*>(component);
		}

		bool HasComponent(uint64_t uuid) {
			if (this->find(uuid) != this->end())
				return true;
			return false;
		}

		void AddComponent(T* component) {
			this->emplace(component->mUuid, *component);
			component = &this->at(component->mUuid);
		}

		void RemoveComponent(T* component) {
			auto it = this->find(component->mUuid);
			if (it != this->end())
				this->erase(this->find(component->mUuid));
		}

		T* InstantiateComponent(uint64_t uuid, T* componentToInstantiate, bool addToComponentsList = true) {
			T* component = nullptr;
			if (this->HasComponent(uuid))
				component = this->GetComponent(uuid);
			else
				component = CopyInstanceFactory(componentToInstantiate);//new T(*componentToInstantiate);

			component->mUuid = uuid;
			component->OnInstantiate(componentToInstantiate);

			std::string className = typeid(T).name();
			if (addToComponentsList) {
				EmplaceFactory(uuid, *component);
				//return EmplaceFactory(uuid, *component);
				//this->emplace(component->mUuid, *CopyInstanceFactory(componentToInstantiate));
				//component = &this->at(component->mUuid);
				//component->mUuid = uuid;
				//component->OnInstantiate(componentToInstantiate);
				//return &this->at(component->mUuid);
			}

			return component;
		}
	};
};