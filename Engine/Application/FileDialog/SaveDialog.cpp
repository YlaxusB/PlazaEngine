#include "Engine/Core/PreCompiledHeaders.h"
#include "FileDialog.h"
// Function to open the file dialog and get the selected file path
namespace Engine {
    std::string FileDialog::SaveFileDialog(const char* filter) {
        OPENFILENAMEA  ofn;
        CHAR  szFileName[260] = { 0 };

        ZeroMemory(&ofn, sizeof(OPENFILENAME));

        ofn.lStructSize = sizeof(OPENFILENAME);
        ofn.hwndOwner = glfwGetWin32Window(Application->Window->glfwWindow);
        ofn.lpstrFile = szFileName;
        ofn.nMaxFile = sizeof(szFileName);
        ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;
        ofn.lpstrFilter = filter;
        ofn.nFilterIndex = 1;

        if (GetSaveFileNameA(&ofn) == TRUE) {
            return ofn.lpstrFile;
        }

        return "";
    }
}