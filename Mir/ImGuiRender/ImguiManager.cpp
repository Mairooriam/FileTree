#include "ImguiManager.h"
#include "FileTree/FileTree.h"
#include "FileTree/Rendering/FileTreeRenderer.h"
#include <memory>
#include <filesystem>

void ImguiManager::Render() {
    
    static std::shared_ptr<FileTree> fTree = std::make_shared<FileTree>();
    static FileTreeRenderer r(fTree);

    static bool callbacksInitialized = false;
    if (!callbacksInitialized)
    {

        r.RegisterExtensionCallback(".exe", FileTreeRenderer::CallbackType::ContextMenu, [](const std::filesystem::path& path) {
            std::cout << "[ANY_CONTEXT] " << path.string() << " - Triggers on context menu for any file without specific handler" << std::endl;
        });
        

        // File callbacks
        r.RegisterFileCallback(FileTreeRenderer::CallbackType::Click, [](const std::filesystem::path& path) {
            std::cout << "[FILE_CLICK] " << path.string() << " - Triggers on any clicked file" << std::endl;
        });
        
        r.RegisterFileCallback(FileTreeRenderer::CallbackType::DoubleClick, [](const std::filesystem::path& path) {
            std::cout << "[FILE_DBLCLICK] " << path.string() << " - Triggers on any double clicked file" << std::endl;
        });
        
        r.RegisterFileCallback(FileTreeRenderer::CallbackType::ContextMenu, [](const std::filesystem::path& path) {
            std::cout << "[FILE_CONTEXT] " << path.string() << " - Triggers on any file context menu" << std::endl;
        });
        
        // Directory callbacks
        r.RegisterDirectoryCallback(FileTreeRenderer::CallbackType::Click, [](const std::filesystem::path& path) {
            std::cout << "[DIR_CLICK] " << path.string() << " - Triggers on any clicked directory" << std::endl;
        });
        
        r.RegisterDirectoryCallback(FileTreeRenderer::CallbackType::DoubleClick, [](const std::filesystem::path& path) {
            std::cout << "[DIR_DBLCLICK] " << path.string() << " - Triggers on any double clicked directory" << std::endl;
        });
        
        r.RegisterDirectoryCallback(FileTreeRenderer::CallbackType::ContextMenu, [](const std::filesystem::path& path) {
            std::cout << "[DIR_CONTEXT] " << path.string() << " - Triggers on any directory context menu" << std::endl;
        });

        // TXT extension callbacks
        r.RegisterExtensionCallback(".txt", FileTreeRenderer::CallbackType::Click, [](const std::filesystem::path& path) {
            std::cout << "[TXT_CLICK] " << path.string() << " - Triggers on any clicked .txt file" << std::endl;
        });
        
        r.RegisterExtensionCallback(".txt", FileTreeRenderer::CallbackType::DoubleClick, [](const std::filesystem::path& path) {
            std::cout << "[TXT_DBLCLICK] " << path.string() << " - Triggers on any double clicked .txt file" << std::endl;
        });
        
        r.RegisterExtensionCallback(".txt", FileTreeRenderer::CallbackType::ContextMenu, [](const std::filesystem::path& path) {
            std::cout << "[TXT_CONTEXT] " << path.string() << " - Triggers on .txt file context menu" << std::endl;
        });

        // TYP extension callbacks
        r.RegisterExtensionCallback(".typ", FileTreeRenderer::CallbackType::Click, [](const std::filesystem::path& path) {
            std::cout << "[TYP_CLICK] " << path.string() << " - Triggers on any clicked .typ file" << std::endl;
        });
        
        // Generic "any" extension callbacks
        r.RegisterExtensionCallback(".any", FileTreeRenderer::CallbackType::Click, [](const std::filesystem::path& path) {
            std::cout << "[ANY_CLICK] " << path.string() << " - Triggers on any clicked file without specific handler" << std::endl;
        });
        
        r.RegisterExtensionCallback(".any", FileTreeRenderer::CallbackType::DoubleClick, [](const std::filesystem::path& path) {
            std::cout << "[ANY_DBLCLICK] " << path.string() << " - Triggers on any double clicked file without specific handler" << std::endl;
        });

        r.RegisterExtensionCallback(".any", FileTreeRenderer::CallbackType::ContextMenu, [](const std::filesystem::path& path) {
            std::cout << "[ANY_CONTEXT] " << path.string() << " - Triggers on context menu for any file without specific handler" << std::endl;
        });
        
        // JSON extension callbacks
        r.RegisterExtensionCallback(".json", FileTreeRenderer::CallbackType::Click, [](const std::filesystem::path& path) {
            std::cout << "[JSON_CLICK] " << path.string() << " - Triggers on any clicked .json file" << std::endl;
        });
        
        r.RegisterExtensionCallback(".json", FileTreeRenderer::CallbackType::DoubleClick, [](const std::filesystem::path& path) {
            std::cout << "[JSON_DBLCLICK] " << path.string() << " - Triggers on any double clicked .json file" << std::endl;
        });
        
        r.RegisterExtensionCallback(".json", FileTreeRenderer::CallbackType::ContextMenu, [](const std::filesystem::path& path) {
            std::cout << "[JSON_CONTEXT] " << path.string() << " - Triggers on .json file context menu" << std::endl;
        });

        callbacksInitialized = true;
    }
    r.Render();

    
}

void glfwErrorCallback(int error, const char* description) {
    // Ignore clipboard conversion errors (65545)
    if (error == 65545 && strstr(description, "Failed to convert clipboard to string") != nullptr) {
        return;
    }
    
    fprintf(stderr, "GLFW Error %d: %s\n", error, description);
}

void ImguiManager::CreateDockspace() {
    static bool dockspaceOpen = true;
    static bool opt_fullscreen = true;

    static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_PassthruCentralNode;

    ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDocking;
    if (opt_fullscreen)
    {
        const ImGuiViewport* viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(viewport->WorkPos);
        ImGui::SetNextWindowSize(viewport->WorkSize);
        ImGui::SetNextWindowViewport(viewport->ID);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
        window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
        window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
    }

    if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
        window_flags |= ImGuiWindowFlags_NoBackground;
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    ImGui::Begin("DockSpace", &dockspaceOpen, window_flags);
    ImGui::PopStyleVar();

    if (opt_fullscreen)
        ImGui::PopStyleVar(2);

    // Submit the DockSpace
    ImGuiIO& io = ImGui::GetIO();
    if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
    {
        ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
        ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
    }
    ImGui::End();
}

ImguiManager::~ImguiManager() {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

bool ImguiManager::initialize() {
    glfwSetErrorCallback(glfwErrorCallback);
    if (!glfwInit())
        return false;

    const char* glsl_version = "#version 130";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

    m_window = glfwCreateWindow(1280, 720, "inputtext callbacktesting", nullptr, nullptr);
    if (m_window == nullptr)
        return false;
    glfwMakeContextCurrent(m_window);
    glfwSwapInterval(1); 

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;  
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;   
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;      
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;    

    ImGui::StyleColorsDark();

    ImGuiStyle& style = ImGui::GetStyle();
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        style.WindowRounding = 0.0f;
        style.Colors[ImGuiCol_WindowBg].w = 1.0f;
    }

    ImGui_ImplGlfw_InitForOpenGL(m_window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);
    return true;
}

void ImguiManager::Begin(){
    glClearColor(0.45f, 0.55f, 0.60f, 1.00f);
    glClear(GL_COLOR_BUFFER_BIT);
   
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    CreateDockspace();
}

void ImguiManager::End(){
    ImGuiIO& io = ImGui::GetIO();
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
        GLFWwindow* backup_current_context = glfwGetCurrentContext();
        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();
        glfwMakeContextCurrent(backup_current_context);
    }
  
    glfwSwapBuffers(m_window);
}



