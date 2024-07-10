#pragma once

namespace Plaza {
	namespace Editor {
		class AssetsImporterWindow : public GuiWindow {
		public:
			AssetsImporterWindow(GuiLayer layer, bool startOpen = true) : GuiWindow(layer, startOpen) {
				
			}

			void Init() override;
			void Update() override;

			void OpenAssetsImporter(std::string filePath, std::string outPath) {
				mFileToImport = filePath;
				outPath = mFileToImportOut;
				this->SetOpen(true);
			}
		private:
			std::string mFileToImport = "";
			std::string mFileToImportOut = "";
			void OnKeyPress(int key, int scancode, int action, int mods) override;
		};
	}
}