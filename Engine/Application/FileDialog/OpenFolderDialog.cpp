#include "Engine/Core/PreCompiledHeaders.h"
#include "FileDialog.h"
#include <ShlObj.h>
// Function to open the file dialog and get the selected file path
namespace Engine {
    std::string FileDialog::OpenFolderDialog() {
        BROWSEINFOA bi;
        ZeroMemory(&bi, sizeof(BROWSEINFO));

        char path[MAX_PATH];
        path[0] = '\0';

        bi.hwndOwner = glfwGetWin32Window(Application->Window->glfwWindow);
        bi.pidlRoot = NULL;
        bi.pszDisplayName = path;
        bi.lpszTitle = "Select a folder";
        bi.ulFlags = BIF_RETURNONLYFSDIRS | BIF_NEWDIALOGSTYLE;

        LPITEMIDLIST pidl = SHBrowseForFolderA(&bi);
        if (pidl != NULL) {
            if (SHGetPathFromIDListA(pidl, path)) {
                // Free the PIDL memory
                IMalloc* imalloc = 0;
                if (SUCCEEDED(SHGetMalloc(&imalloc))) {
                    imalloc->Free(pidl);
                    imalloc->Release();
                }

                return path;
            }
        }

        return "";
    }
}