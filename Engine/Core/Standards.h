#pragma once
namespace Plaza {
	using namespace std;
	class Standards {
	public:
		static inline string engineExtName = ".plz";
		static inline string projectExtName = engineExtName + "prj";
		static inline string sceneExtName = engineExtName + "scn";
		static inline string materialExtName = engineExtName + "mat";
		static inline string modelExtName = engineExtName + "mod";
		static inline string scriptConfigExtName = engineExtName + "scp";
		static inline string metadataExtName = engineExtName + "meta";
		static inline string animationExtName = engineExtName + "anim";
	};
}