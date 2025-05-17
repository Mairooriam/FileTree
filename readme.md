# FileTree
Imgui filetree with for getting selected file and doing action trough contextmenu. Has some badly implemented callbacks onClick, onDoubleClick and onContextMenu with extension specific variants. If u wanna use this i recommend stripping away the callback system and actually making something good or using ImGui stuff raw.

### Why? - Learning
- abstracting and making API for interraction
- Callbacks
- Data structures (trees)

### Goals
- Filetree that supplies my application selected path / file without giving access to implementation.
- Ability to add functionality for extension specific selection.
- File Extension specific contextMenu
    - Not implemented fully.
- Change root directory

### TODOs
- Ability to supply whole contextmenu for whatever file extension
    - FileTreeRenderer.setCallbackContextMenuConfig()
- Better way to implement FileTreeRenderer API. Don't like current callback thing.
- FileTree improvements to implementation.
- Non Windows specific FileDialog -> Maybe https://github.com/aiekick/ImGuiFileDialog?

# Building
Dependencies:
- **Windows** - For WindowsFileDialog.cpp
- **Dear ImGui** - For Main functionality
- **GLFW** - for ImGui backend
- **OpenGL** - for ImGUi backend
- **C++23** - for std::string.contains(), **C++17** std::filsystem

The project uses CMake to manage dependencies and the build process. 

## Step-by-Step Build Guide

### Prerequisites
1. Install [CMake](https://cmake.org/download/) (3.10 or newer)
2. Install a C++23 compatible compiler (e.g., Visual Studio 2022, GCC 13+, or Clang 16+)
3. Make sure you have OpenGL development libraries installed (typically included with graphics drivers)

### Building with CMake

#### Command Line
1. Clone the repository:
   ```bash
   git clone https://github.com/Mairooriam/FileTree.git
   cd FileTree
   ```

2. Create a build directory:
   ```bash
   mkdir build
   cd build
   ```

3. Configure the project:
   ```bash
   cmake ..
   ```

4. Build the project:
   ```bash
   cmake --build . --config Release
   ```

5. Run the example:
   ```bash
   ./Mir/example
   ```


# Rendering
There are some behavior that isnt controller trough the callbacks. For example double clicking a file in filetree will open readonly preview of the file.
```cpp
static std::shared_ptr<FileTree> fTree = std::make_shared<FileTree>();
static FileTreeRenderer r(fTree);
r.Render();
```
![File Tree Screenshot](Resources/example.png)
# FileTree
Too bad no information about filetree. Defaults to current project directory when constructred.
# Callback examples
Bad implementation of a callback system. Split into two: **General** and **Extension specific**
## Extension Callback Example 

Incase you add **RegisterExtensionCallback** the filetree will automatically add ImGui::MenuItem to ImGui::ContextMenu in [File Tree Context Menu Implementation](rendering/FileTreeRenderer.cpp#L132). You can also see this in the See the Check out the [example][screenshot1] for a visual example. <span style="color:red">red</span> and <span style="color:magenta">purple</span> boxes compares effect of this functionality.
```cpp
r.RegisterExtensionCallback(".any", FileTreeRenderer::CallbackType::ContextMenu, [](const std::filesystem::path& path) {
    std::cout << "[ANY_CONTEXT] " << path.string() << " - Triggers on context menu for any file without specific handler" << std::endl;
});

```
## General file callbacks
```cpp
// File callbacks
r.RegisterFileCallback(FileTreeRenderer::CallbackType::Click, [](const std::filesystem::path& path) {});
r.RegisterFileCallback(FileTreeRenderer::CallbackType::DoubleClick, [](const std::filesystem::path& path) {});
r.RegisterFileCallback(FileTreeRenderer::CallbackType::ContextMenu, [](const std::filesystem::path& path) {});
```

## General directory callbacks
```cpp
// Director callbacks
r.RegisterDirectoryCallback(FileTreeRenderer::CallbackType::Click, [](const std::filesystem::path& path) {});
r.RegisterDirectoryCallback(FileTreeRenderer::CallbackType::DoubleClick, [](const std::filesystem::path& path) {});
r.RegisterDirectoryCallback(FileTreeRenderer::CallbackType::ContextMenu, [](const std::filesystem::path& path) {});
```


## Extension specific callbacks 
```cpp
// Generic "any" extension callbacks
r.RegisterExtensionCallback(".any", FileTreeRenderer::CallbackType::Click, [](const std::filesystem::path& path) {});
r.RegisterExtensionCallback(".any", FileTreeRenderer::CallbackType::DoubleClick, [](const std::filesystem::path& path) {});
r.RegisterExtensionCallback(".any", FileTreeRenderer::CallbackType::ContextMenu, [](const std::filesystem::path& path) {});
```
## Licenses

This project is licensed under the MIT License - see the [LICENSE.txt](LICENSE.txt) file for details.

### Third-Party Libraries

This project uses the following third-party libraries:

* **Dear ImGui** - MIT License - [View License](Resources/Licenses/imgui-LICENSE.txt)
* **GLFW** - zlib/libpng License - [View License](Resources/Licenses/glfw-LICENSE.txt)

[screenshot1]: example.png