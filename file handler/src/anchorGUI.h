#pragma once
#include <windows.h>
#include <commctrl.h>
#include <shlwapi.h>
#include <shlobj.h>
#include <strsafe.h>
#include <vector>
#include <string>
#include <uxtheme.h>
#include <mmsystem.h>
#include <windowsx.h>
#include <wininet.h>
#include <pdh.h>
#include <thread>
#include <chrono>

#pragma comment(lib, "comctl32.lib")
#pragma comment(lib, "Shlwapi.lib")
#pragma comment(lib, "Shell32.lib")
#pragma comment(lib, "uxtheme.lib")
#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "wininet.lib")
#pragma comment(lib, "pdh.lib")

// Forward declarations
HWND g_hMainWindow = NULL;
HWND g_hPathEdit = NULL;
HWND g_hListView = NULL;
HWND g_hStatusBar = NULL;
HWND g_hCmdEdit = NULL;
HWND g_hCmdOutput = NULL;
std::wstring g_currentPath;
std::wstring g_cmdHistory;
HINSTANCE g_hInstance = NULL;
HIMAGELIST g_hImageList = NULL;
HFONT g_hFont = NULL;
HFONT g_hHackerFont = NULL;
HFONT g_hHackerFontSmall = NULL;
HWND g_hAsciiBanner = NULL;
HWND g_hMatrixEffect = NULL;
HWND hwnd = NULL;
HWND g_hSideBar = NULL;
SYSTEMTIME g_startTime;


// Control IDs
#define IDC_PATH_EDIT      1001
#define IDC_GO_BUTTON      1002
#define IDC_UP_BUTTON      1003
#define IDC_LISTVIEW       1004
#define IDC_STATUSBAR      1005
#define IDC_CMD_EDIT       1006
#define IDC_CMD_OUTPUT    1007
#define IDC_CMD_ENTER      1008
#define IDC_STATUS_BOX     1009
#define IDC_STATUS_TIMER   1010
#define IDC_SIDEBAR        1011

// Custom messages
#define WM_POPULATE_LIST   (WM_USER + 1)

// Menu IDs
#define IDM_FILE_NEW       101
#define IDM_FILE_COPY      102
#define IDM_FILE_DELETE    103
#define IDM_FILE_RENAME    104
#define IDM_FILE_PROPERTIES 105
#define IDM_FILE_EXIT      106
#define IDM_VIEW_REFRESH   201

// Function declarations
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
bool InitializeApplication(_In_ HINSTANCE hInstance);
bool CreateMainWindow(_In_ HINSTANCE hInstance, _In_ int nCmdShow);
void CreateMenus(_In_ HWND hwnd);
void CreateControls(_In_ HWND hwnd);
void ResizeControls(_In_ HWND hwnd);
void InitializeListViewColumns(_In_ HWND hListView);
void InitializeImageList(void);
int GetIconIndex(_In_ const std::wstring& filePath, _In_ bool isDirectory);
void PopulateListView(_In_ HWND hListView, _In_ const std::wstring& path);
void NavigateToPath(_In_ const std::wstring& path);
void NavigateUp(void);
void ShowFileProperties(_In_ const std::wstring& filePath);
void DeleteSelectedFiles(void);
void CopySelectedFiles(void);
void RenameSelectedFile(void);
void CreateNewFile(void);
void UpdateStatusBar(_In_ HWND hListView);
void CreateHackerFont(void);
void AddAsciiArt(_In_ HWND hwnd);
void CreateMatrixEffect(_In_ HWND hwnd);
void DrawMatrixEffect(_In_ HDC hdc, _In_ RECT rc);
void InitializeMatrixEffect(void);
void SetWindowTextColor(_In_ HWND hwnd, _In_ COLORREF color);
void PlayKeySound(void);
void PlayErrorSound(void);
void PlaySuccessSound(void);
HANDLE AddResource(_In_ HINSTANCE hInstance, _In_ LPVOID lpData, _In_ DWORD dwSize, _In_ LPCWSTR lpName, _In_ LPCWSTR lpType);
LRESULT CALLBACK CmdEditSubclassProc(_In_ HWND hwnd, _In_ UINT uMsg, _In_ WPARAM wParam, _In_ LPARAM lParam, _In_ UINT_PTR uIdSubclass, _In_ DWORD_PTR dwRefData);
void ExecuteCommand(void);
void UpdateStatusBox(void);
void GetSystemTemperatures(_Out_ std::wstring& cpuTemp, _Out_ std::wstring& gpuTemp);
void GetNetworkStatus(_Out_ std::wstring& status);
void GetUptime(_Out_ std::wstring& uptime);
std::wstring GetFormattedPrompt(void);
