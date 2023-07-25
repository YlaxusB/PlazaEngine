#include "Engine/Core/PreCompiledHeaders.h"
#include "FileDialog.h"
#include <ShlObj.h>
#include <ShObjIdl.h>

// Function to open the file dialog and get the selected file path
namespace Engine {
    std::string FileDialog::OpenFolderDialog() {
        std::string selectedFolder;

        CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);

        IFileDialog* pFolderDialog;
        HRESULT hr = CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&pFolderDialog));

        if (SUCCEEDED(hr)) {
            DWORD dwOptions;
            pFolderDialog->GetOptions(&dwOptions);
            pFolderDialog->SetOptions(dwOptions | FOS_PICKFOLDERS | FOS_FORCEFILESYSTEM);

            hr = pFolderDialog->Show(NULL);

            if (SUCCEEDED(hr)) {
                IShellItem* pItem;
                hr = pFolderDialog->GetResult(&pItem);
                if (SUCCEEDED(hr)) {
                    PWSTR pszFolderPath;
                    hr = pItem->GetDisplayName(SIGDN_FILESYSPATH, &pszFolderPath);
                    if (SUCCEEDED(hr)) {
                        int bufferSize = WideCharToMultiByte(CP_UTF8, 0, pszFolderPath, -1, NULL, 0, NULL, NULL);
                        if (bufferSize > 0) {
                            char* buffer = new char[bufferSize];
                            WideCharToMultiByte(CP_UTF8, 0, pszFolderPath, -1, buffer, bufferSize, NULL, NULL);
                            selectedFolder = buffer;
                            delete[] buffer;
                        }
                        CoTaskMemFree(pszFolderPath);
                    }
                    pItem->Release();
                }
            }

            pFolderDialog->Release();
        }

        CoUninitialize();

        return selectedFolder;
    }
    /*
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
    }*/
}