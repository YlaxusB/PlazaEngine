#pragma once
#include <variant>
#include <any>
#include <iostream>
#include <typeinfo>
#include <typeindex>

namespace Plaza {
	class Any {
	public:
		constexpr Any() noexcept
			: mType(&typeid(void)) {}

		~Any() {
			Reset();
		}

		template<typename T>
		void SetValue(T value, bool changeType = true) {
			mValue = static_cast<void*>(new T(value));

			if (changeType)
				mType = &typeid(T);
		}

		template<typename T>
		void SetValuePtr(T* value, bool changeType = true) {
			mValue = value;//static_cast<void*>(new T(value));

			if (changeType)
				mType = &typeid(T);
		}

		template<typename T>
		void SetType() {
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
			//    delete static_cast<void*>(mValue); // Type-safe deletion
			mValue = nullptr;
			mType = &typeid(void);
		}

	private:
		void* mValue = nullptr;
		const std::type_info* mType;
	};
}