#include "WindowsFileDialog.h"
#include <codecvt>
#include <locale>
namespace Mir
{
    // straight from Copilot 
    WindowsFileDialog::WindowsFileDialog() : m_isInitialized(false) {}
    
    WindowsFileDialog::~WindowsFileDialog() {
        if (m_isInitialized) {
            ReleaseCOM();
        }
    }
    
    bool WindowsFileDialog::InitializeCOM() {
        HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
        m_isInitialized = SUCCEEDED(hr);
        return m_isInitialized;
    }
    
    void WindowsFileDialog::ReleaseCOM() {
        if (m_isInitialized) {
            CoUninitialize();
            m_isInitialized = false;
        }
    }
    
    std::wstring WindowsFileDialog::Utf8ToWide(const std::string& str) {
        try {
            std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
            return converter.from_bytes(str);
        } catch (const std::exception&) {
            return std::wstring(str.begin(), str.end());
        }
    }
    
    void WindowsFileDialog::OpenFolderDialog(const std::string& title, FileDialogCallback callback) {
        if (!m_isInitialized && !InitializeCOM()) {
            return;
        }
        
        IFileDialog* pfd = NULL;
        HRESULT hr = CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_INPROC_SERVER,
                                     IID_PPV_ARGS(&pfd));
        
        if (SUCCEEDED(hr)) {
            // Set options for folder picker
            DWORD dwOptions;
            hr = pfd->GetOptions(&dwOptions);
            if (SUCCEEDED(hr)) {
                hr = pfd->SetOptions(dwOptions | FOS_PICKFOLDERS);
                
                // Set title if specified
                if (!title.empty()) {
                    std::wstring wideTitle = Utf8ToWide(title);
                    pfd->SetTitle(wideTitle.c_str());
                }
                
                // Set initial folder if specified
                if (!m_initialPath.empty()) {
                    IShellItem* psiFolder = NULL;
                    hr = SHCreateItemFromParsingName(m_initialPath.c_str(), NULL, IID_PPV_ARGS(&psiFolder));
                    if (SUCCEEDED(hr)) {
                        pfd->SetFolder(psiFolder);
                        psiFolder->Release();
                    }
                }
                
                if (SUCCEEDED(hr)) {
                    // Show the dialog
                    hr = pfd->Show(NULL);
                    
                    if (SUCCEEDED(hr)) {
                        // Get the folder path
                        IShellItem* psi = NULL;
                        hr = pfd->GetResult(&psi);
                        
                        if (SUCCEEDED(hr)) {
                            PWSTR pszPath = NULL;
                            hr = psi->GetDisplayName(SIGDN_FILESYSPATH, &pszPath);
                            
                            if (SUCCEEDED(hr)) {
                                std::filesystem::path selectedPath(pszPath);
                                // Execute the callback with the selected path
                                callback(selectedPath);
                                CoTaskMemFree(pszPath);
                            }
                            psi->Release();
                        }
                    }
                }
            }
            pfd->Release();
        }
    }
    
    void WindowsFileDialog::OpenFileDialog(const std::string& title, FileDialogCallback callback, 
                                           const std::string& filter) {
        if (!m_isInitialized && !InitializeCOM()) {
            return;
        }
        
        IFileDialog* pfd = NULL;
        HRESULT hr = CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_INPROC_SERVER,
                                     IID_PPV_ARGS(&pfd));
        
        if (SUCCEEDED(hr)) {
            // Set options for file picker
            DWORD dwOptions;
            hr = pfd->GetOptions(&dwOptions);
            if (SUCCEEDED(hr)) {
                // No FOS_PICKFOLDERS here since we're selecting a file
                
                // Set title if specified
                if (!title.empty()) {
                    std::wstring wideTitle = Utf8ToWide(title);
                    pfd->SetTitle(wideTitle.c_str());
                }
                
                // Set file filters if specified
                if (!filter.empty()) {
                    // Parse the filter string - simple implementation for now
                    // Format: "Description|*.ext1;*.ext2|Description2|*.ext3"
                    // ... (filter parsing code would go here)
                }
                
                // Set initial folder if specified
                if (!m_initialPath.empty()) {
                    IShellItem* psiFolder = NULL;
                    hr = SHCreateItemFromParsingName(m_initialPath.c_str(), NULL, IID_PPV_ARGS(&psiFolder));
                    if (SUCCEEDED(hr)) {
                        pfd->SetFolder(psiFolder);
                        psiFolder->Release();
                    }
                }
                
                if (SUCCEEDED(hr)) {
                    // Show the dialog
                    hr = pfd->Show(NULL);
                    
                    if (SUCCEEDED(hr)) {
                        // Get the file path
                        IShellItem* psi = NULL;
                        hr = pfd->GetResult(&psi);
                        
                        if (SUCCEEDED(hr)) {
                            PWSTR pszPath = NULL;
                            hr = psi->GetDisplayName(SIGDN_FILESYSPATH, &pszPath);
                            
                            if (SUCCEEDED(hr)) {
                                std::filesystem::path selectedPath(pszPath);
                                // Execute the callback with the selected path
                                callback(selectedPath);
                                CoTaskMemFree(pszPath);
                            }
                            psi->Release();
                        }
                    }
                }
            }
            pfd->Release();
        }
    }
    
    void WindowsFileDialog::SetInitialPath(const std::filesystem::path& path) {
        m_initialPath = path.wstring();
    }
} // namespace Mir
