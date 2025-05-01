# primitive-file-handler
F1L3 3XP10R3R is a highly customized and visually enhanced Windows file explorer written in modern C++ with native Win32 APIs. It features a matrix-themed UI, real-time system stats, a built-in command prompt, and support for file operations like copy, delete, rename, and execute.
# 🧠 F1L3 3XP10R3R - Win32 File Explorer (from scratch in C++)

F1L3 3XP10R3R (pronounced "File Explorer") is a **custom-built file manager and command interface** for Windows, designed from the ground up using **pure Win32 API in C++**. It combines file system navigation, command-line interaction, and a unique hacker-style UI reminiscent of classic terminal aesthetics.

> ✅ Built 100% from scratch using **native Win32 API** — no frameworks, no MFC, no .NET  
> ✅ ⚠️ Works only on **Windows platforms** (Windows 7+ recommended)

---

## 📸 Preview

> *(Add screenshots or a demo GIF here if available)*

---

## 🚀 Features

- ✅ **Custom File Explorer UI**
  - Navigate through files and folders
  - Displays name, size, type, date modified, and icons
  - Supports list sorting and full-row selection

- 💻 **Integrated Command Prompt**
  - Execute shell commands directly from the app
  - Supports internal commands: `cls`, `exit`
  - Recognizes `python` and `py` commands (if Python is in PATH)
  - Simulates `cmd.exe` behavior with output piping

- 🧪 **System Monitoring**
  - Real-time CPU usage monitoring using **PDH**
  - Simulated GPU temperature (based on CPU load)
  - Live uptime and system info display

- 🎨 **Matrix-Style Hacker UI**
  - ASCII art banner at the top
  - Green-on-black theme throughout
  - "Matrix rain" animation in dedicated panel

- 🗃️ **File Operations**
  - Create new files
  - Copy to clipboard
  - Rename or delete files/folders
  - Show properties dialog
  - Open files with default programs

- 🔊 **Audio Feedback**
  - Success, warning, and error sounds using `PlaySound`

---

## 🛠️ Tech Stack

| Component        | Description                        |
|------------------|------------------------------------|
| Language         | C++ (Visual Studio)                |
| API              | Win32 API (native Windows GUI)     |
| Libraries Used   | `comctl32.lib`, `shlwapi.lib`, `shell32.lib`, `uxtheme.lib`, `winmm.lib`, `wininet.lib`, `pdh.lib` |
| Target OS        | Windows only (Windows 7 or higher) |

---

## 🏗️ How to Build

### Requirements
- Visual Studio (2017 or newer)
- Windows OS
- C++ workload installed in Visual Studio

### Steps
1. Clone this repository.
2. Open the `.cpp` file as a new project or add it to an empty **Win32 Desktop App** project in Visual Studio.
3. Link the required libraries in **Project Properties > Linker > Input > Additional Dependencies**:
   ```text
   comctl32.lib; shlwapi.lib; shell32.lib; uxtheme.lib; winmm.lib; wininet.lib; pdh.lib;
