#pragma once

namespace Plaza {
	enum class SerializationMode {
		SERIALIZE_BINARY,
		SERIALIZE_JSON,
		SERIALIZE_YAML
	};
	class EngineSettings {
	public:
		SerializationMode mCommonSerializationMode = SerializationMode::SERIALIZE_JSON;
		SerializationMode mMetaDataSerializationMode = SerializationMode::SERIALIZE_JSON;
		SerializationMode mSceneSerializationMode = SerializationMode::SERIALIZE_JSON;
		SerializationMode mProjectSerializationMode = SerializationMode::SERIALIZE_JSON;
		SerializationMode mSettingsSerializationMode = SerializationMode::SERIALIZE_JSON;
		SerializationMode mModelSerializationMode = SerializationMode::SERIALIZE_BINARY;
		SerializationMode mAnimationSerializationMode = SerializationMode::SERIALIZE_BINARY;
		SerializationMode mMaterialSerializationMode = SerializationMode::SERIALIZE_JSON;
	};
}