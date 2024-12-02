#pragma once
#include <variant>
#include <any>
#include <iostream>
#include <typeinfo>
#include <typeindex>
#include "Engine/Core/Engine.h"

namespace Plaza {
	class Any {
	public:
		Any() noexcept
			: mType(&typeid(void)) {}

		~Any() {
			Reset();
		}

		template<typename T>
		void SetValue(T value, bool changeType = true) {
			mValue = static_cast<void*>(new T(value));

			if (changeType) {
				mSize = sizeof(T);
				this->RegisterType<T>();
				mType = &typeid(T);
			}
		}

		template<typename T>
		void SetValuePtr(T* value, bool changeType = true) {
			mValue = value;//static_cast<void*>(new T(value));

			if (changeType) {
				mSize = sizeof(T);
				this->RegisterType<T>();
				mType = &typeid(T);
			}
		}

		template<typename T>
		void SetType() {
			this->RegisterType<T>();
			mType = &typeid(T);
		}

		template<typename T>
		T* GetValue() const {
			if constexpr (std::is_reference_v<T>) {
				return static_cast<T*>(mValue); // Return the address for references
			}
			else {
				return static_cast<T*>(mValue);
			}
		}

		template <typename T>
		Any& operator=(T&& value) {
			SetValue(std::forward<T>(value), true);
			return *this;
		}

		const std::type_info& type() const { return *mType; }

		void Reset() {
			//if (mValue)
			//	delete mValue; // Type-safe deletion
			mValue = nullptr;
			mType = &typeid(void);
		}

		void* mValue = nullptr;
		uintptr_t mSize = 0;

		template <typename T>
		struct always_false : std::false_type {};
		template <class Archive>
		void serialize(Archive& archive) {
			if (mValue == nullptr)
				return;

			if constexpr (std::is_same_v<Archive, cereal::JSONOutputArchive> ||
				std::is_same_v<Archive, cereal::JSONInputArchive>) {
				// Handle JSON-specific serialization or deserialization
				if constexpr (Archive::is_saving::value) {
					// JSON serialization
					// Example: archive(value);
					mJsonSerialize(archive, mValue);
				}
				else if constexpr (Archive::is_loading::value) {
					// JSON deserialization
					// Example: archive(value);
					mJsonDeSerialize(archive, mValue);
				}
			}
			else if constexpr (std::is_same_v<Archive, cereal::BinaryOutputArchive> ||
				std::is_same_v<Archive, cereal::BinaryInputArchive>) {
				// Handle Binary-specific serialization or deserialization
				if constexpr (Archive::is_saving::value) {
					// Binary serialization
					// Example: archive(value);
					mBinarySerialize(archive, mValue);
				}
				else if constexpr (Archive::is_loading::value) {
					// Binary deserialization
					// Example: archive(value);
					mBinaryDeSerialize(archive, mValue);
				}
			}
			else {
				static_assert(always_false<Archive>::value, "Unsupported archive type");
			}
		}

		template<typename T>
		void RegisterType() {
			// Setup serialization
			mJsonSerialize = [this](cereal::JSONOutputArchive& archive, void* value) {
				T* newValue = static_cast<T*>(value);
				archive(*newValue); // Serialize the value
				};
			mBinarySerialize = [this](cereal::BinaryOutputArchive& archive, void* value) {
				T* newValue = static_cast<T*>(value);
				archive(*newValue); // Serialize the value
				};

			// Setup deserialization
			mJsonDeSerialize = [this](cereal::JSONInputArchive& archive, void* value) {
				T newValue;
				archive(newValue); // Deserialize into newValue
				this->SetValue(newValue, false); // Store the new value
				};
			mBinaryDeSerialize = [this](cereal::BinaryInputArchive& archive, void* value) {
				T newValue;
				archive(newValue); // Deserialize into newValue
				this->SetValue(newValue, false); // Store the new value
				};
		}

		std::function<void(cereal::JSONOutputArchive&, void*)> mJsonSerialize;
		std::function<void(cereal::JSONInputArchive&, void*)> mJsonDeSerialize;

		std::function<void(cereal::BinaryOutputArchive&, void*)> mBinarySerialize;
		std::function<void(cereal::BinaryInputArchive&, void*)> mBinaryDeSerialize;
	private:
		const std::type_info* mType;
	};
}