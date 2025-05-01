#include "anchorGUI.h"
#include "resource.h"

// Temperature Monitor class definition
class TemperatureMonitor {
private:
    PDH_HQUERY cpuQuery;
    PDH_HCOUNTER cpuTotal;
    ULONGLONG lastCPUTime;
    float currentCPU;
    float currentGPU;
    bool initialized;

public:
    TemperatureMonitor() : lastCPUTime(0), currentCPU(0), currentGPU(0), initialized(false) {
        // Initialize CPU performance counter
        PdhOpenQuery(NULL, NULL, &cpuQuery);
        PdhAddCounter(cpuQuery, L"\\Processor(_Total)\\% Processor Time", NULL, &cpuTotal);
        PdhCollectQueryData(cpuQuery);
        initialized = true;
        
        // Start update thread
        std::thread([this]() {
            while (true) {
                updateTemperatures();
                std::this_thread::sleep_for(std::chrono::seconds(1));
            }
        }).detach();
    }

    ~TemperatureMonitor() {
        if (initialized) {
            PdhCloseQuery(cpuQuery);
        }
    }

private:
    void updateTemperatures() {
        if (!initialized) return;

        // Update CPU usage
        PDH_FMT_COUNTERVALUE counterVal;
        PdhCollectQueryData(cpuQuery);
        PdhGetFormattedCounterValue(cpuTotal, PDH_FMT_DOUBLE, NULL, &counterVal);
        
        // Use CPU usage to simulate temperature (30°C + usage-based variation)
        currentCPU = 30.0f + (float)(counterVal.doubleValue * 0.7);
        
        // Simulate GPU temperature based on CPU with some variation
        static float direction = 0.1f;
        currentGPU += direction;
        if (currentGPU > currentCPU + 10) direction = -0.1f;
        if (currentGPU < currentCPU - 10) direction = 0.1f;
        
        // Keep temperatures in reasonable range
        if (currentCPU > 90.0f) currentCPU = 90.0f;
        if (currentCPU < 30.0f) currentCPU = 30.0f;
        if (currentGPU > 85.0f) currentGPU = 85.0f;
        if (currentGPU < 35.0f) currentGPU = 35.0f;
    }

public:
    bool getTemperatures(std::wstring& cpu, std::wstring& gpu) {
        if (!initialized) {
            cpu = L"N/A";
            gpu = L"N/A";
            return false;
        }

        wchar_t temp[32];
        StringCchPrintf(temp, 32, L"%.1f°C", currentCPU);
        cpu = temp;
        StringCchPrintf(temp, 32, L"%.1f°C", currentGPU);
        gpu = temp;
        return true;
    }
};

// Global temperature monitor instance
TemperatureMonitor g_temperatureMonitor;



// Window class name and title
const wchar_t* APP_CLASS_NAME = L"Win32FileExplorerClass";
const wchar_t* APP_TITLE = L"F1L3 3XP10R3R";

// ASCII Art for the banner
const wchar_t* ASCII_BANNER = L""
L"  _    _            _    _____ _ _      ______ _           _\n"
L" | |  | |          | |  |  __ (_) |     |  ___(_)         | |\n"
L" | |__| | __ _  ___| | _| |__) |_| |_   | |_   _ _ __ ___ | |\n"
L" |  __  |/ _` |/ __| |/ /  ___/ | __|  |  _| | | '_ ` _ \\| |\n"
L" | |  | | (_| | (__|   <| |   | | |_   | |   | | | | | | |_|\n"
L" |_|  |_|\\__,_|\\___|_|\\_\\_|   |_|\\__|  \\_|   |_|_| |_| |_(_)\n";

// Global variables


// Matrix effect variables
const int MATRIX_COLUMNS = 80;
const int MATRIX_ROWS = 10;
wchar_t matrixChars[MATRIX_COLUMNS][MATRIX_ROWS];
int matrixSpeeds[MATRIX_COLUMNS];
int matrixPositions[MATRIX_COLUMNS];

// Add EM_SETBKGNDCOLOR definition if not already defined
#ifndef EM_SETBKGNDCOLOR
#define EM_SETBKGNDCOLOR (WM_USER + 67)
#endif

// Rename Dialog Template
#ifndef RT_DIALOG
#define RT_DIALOG MAKEINTRESOURCE(5)
#endif

static const unsigned char g_RenameDialogTemplate[] = {
    0x01, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xc8, 0x00, 0xc8, 0x90,
    0x05, 0x00, 0x00, 0x00, 0x00, 0x00, 0x92, 0x00, 0x2e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x52, 0x00,
    0x65, 0x00, 0x6e, 0x00, 0x61, 0x00, 0x6d, 0x00, 0x65, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 0x00,
    0x00, 0x01, 0x4d, 0x00, 0x53, 0x00, 0x20, 0x00, 0x53, 0x00, 0x68, 0x00, 0x65, 0x00, 0x6c, 0x00,
    0x6c, 0x00, 0x20, 0x00, 0x44, 0x00, 0x6c, 0x00, 0x67, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x02, 0x50, 0x06, 0x00, 0x0a, 0x00, 0x0a, 0x00, 0x7e, 0x00, 0x0e, 0x00, 0x01, 0x00, 0x00, 0x00,
    0xff, 0xff, 0x82, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x02, 0x50, 0x06, 0x00, 0x20, 0x00, 0x7e, 0x00, 0x12, 0x00, 0x65, 0x00, 0x02, 0x00, 0x01, 0x00,
    0xff, 0xff, 0x81, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x02, 0x50, 0x06, 0x00, 0x35, 0x00, 0x18, 0x00, 0x12, 0x00, 0x01, 0x00, 0x03, 0x00, 0x01, 0x00,
    0xff, 0xff, 0x80, 0x00, 0x4f, 0x00, 0x4b, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x02, 0x50, 0x06, 0x00, 0x53, 0x00, 0x18, 0x00, 0x12, 0x00, 0x02, 0x00, 0x04, 0x00, 0x00, 0x00,
    0xff, 0xff, 0x80, 0x00, 0x43, 0x00, 0x61, 0x00, 0x6e, 0x00, 0x63, 0x00, 0x65, 0x00, 0x6c, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

// Register the dialog template
void RegisterDialogTemplate(HINSTANCE hInstance) {
    HANDLE hResource = AddResource(
        hInstance,
        (LPVOID)g_RenameDialogTemplate,
        sizeof(g_RenameDialogTemplate),
        L"RENAME_DIALOG",
        RT_DIALOG
    );
}

// Helper function to add a resource
HANDLE AddResource(_In_ HINSTANCE hInstance, _In_ LPVOID lpData, _In_ DWORD dwSize, _In_ LPCWSTR lpName, _In_ LPCWSTR lpType) {
    HANDLE hResource = NULL;
    return hResource;
}

// Add this function to handle sidebar painting
LRESULT CALLBACK SideBarProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData) {
    switch (uMsg) {
        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);
            
            // Set up double buffering
            RECT rect;
            GetClientRect(hwnd, &rect);
            HDC hdcMem = CreateCompatibleDC(hdc);
            HBITMAP hbmMem = CreateCompatibleBitmap(hdc, rect.right - rect.left, rect.bottom - rect.top);
            HBITMAP hbmOld = (HBITMAP)SelectObject(hdcMem, hbmMem);
            
            // Fill background
            HBRUSH hBrush = CreateSolidBrush(RGB(0, 0, 0));
            FillRect(hdcMem, &rect, hBrush);
            DeleteObject(hBrush);
            
            // Set text properties
            SetBkMode(hdcMem, TRANSPARENT);
            SetTextColor(hdcMem, RGB(0, 255, 0));
            SelectObject(hdcMem, g_hHackerFontSmall);
            
            // Get the text to display
            wchar_t buffer[2048];
            GetWindowText(hwnd, buffer, 2048);
            
            // Draw the text
            DrawText(hdcMem, buffer, -1, &rect, DT_LEFT | DT_WORDBREAK);
            
            // Copy the off-screen buffer to the screen
            BitBlt(hdc, 0, 0, rect.right - rect.left, rect.bottom - rect.top, hdcMem, 0, 0, SRCCOPY);
            
            // Clean up
            SelectObject(hdcMem, hbmOld);
            DeleteObject(hbmMem);
            DeleteDC(hdcMem);
            EndPaint(hwnd, &ps);
            return 0;
        }
        case WM_ERASEBKGND:
            return 1; // Prevent flickering
    }
    return DefSubclassProc(hwnd, uMsg, wParam, lParam);
}

// Add proper annotation for wWinMain
int WINAPI wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow) {
    // Initialize COM
    HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
    if (FAILED(hr)) {
        return -1;
    }

    g_hInstance = hInstance;

    // Initialize common controls
    INITCOMMONCONTROLSEX icex;
    icex.dwSize = sizeof(INITCOMMONCONTROLSEX);
    icex.dwICC = ICC_LISTVIEW_CLASSES | ICC_BAR_CLASSES;
    InitCommonControlsEx(&icex);

    // Initialize the application
    if (!InitializeApplication(hInstance)) {
        CoUninitialize();
        return -1;
    }

    // Register dialog templates - Note: in a real app these would be in resources
    // RegisterDialogTemplate(hInstance);  // Commented out as it won't work in this example

    // Create the main window
    if (!CreateMainWindow(hInstance, nCmdShow)) {
        CoUninitialize();
        return -1;
    }

    // Message loop
    MSG msg = {};
    while (GetMessage(&msg, NULL, 0, 0)) {
        if (!IsDialogMessage(g_hMainWindow, &msg)) {  // Add dialog message handling
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    // Clean up
    ImageList_Destroy(g_hImageList);
    CoUninitialize();

    return (int)msg.wParam;
}

// Initialize the application
bool InitializeApplication(_In_ HINSTANCE hInstance) {
    // Get the instance handle
    g_hInstance = GetModuleHandle(NULL);
    if (!g_hInstance) {
        MessageBox(NULL, L"Failed to get module handle!", L"Error", MB_ICONERROR | MB_OK);
        return FALSE;
    }

    // Test with system icon first
    HICON hIcon = LoadIcon(NULL, MAKEINTRESOURCE((WORD)IDI_ERROR));
    HICON hIconSm = LoadIcon(NULL, MAKEINTRESOURCE((UINT_PTR)IDI_ERROR));

    if (!hIcon || !hIconSm) {
        MessageBox(NULL, L"Failed to load system icons!", L"Error", MB_ICONERROR | MB_OK);
        return FALSE;
    }

    WNDCLASSEX wcex = { 0 };
    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WindowProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = g_hInstance;
    wcex.hIcon = hIcon;
    wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
    wcex.hbrBackground = CreateSolidBrush(RGB(0, 0, 0));
    wcex.lpszMenuName = NULL;
    wcex.lpszClassName = APP_CLASS_NAME;
    wcex.hIconSm = hIconSm;

    if (!RegisterClassEx(&wcex)) {
        MessageBox(NULL, L"Window Registration Failed!", L"Error", MB_ICONEXCLAMATION | MB_OK);
        return FALSE;
    }

    return TRUE;
}

// Create the main window
bool CreateMainWindow(_In_ HINSTANCE hInstance, _In_ int nCmdShow) {
    g_hMainWindow = CreateWindowEx(
        0,
        APP_CLASS_NAME,
        APP_TITLE,
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT,
        800, 600,
        NULL,
        NULL,
        g_hInstance,
        NULL
    );

    if (!g_hMainWindow) {
        MessageBox(NULL, L"Window Creation Failed!", L"Error", MB_ICONEXCLAMATION | MB_OK);
        return FALSE;
    }

    // Set the window icons using system icons
    HICON hIcon = LoadIcon(NULL, MAKEINTRESOURCE((UINT_PTR)IDI_ERROR));
    HICON hIconSm = LoadIcon(NULL, MAKEINTRESOURCE((UINT_PTR)IDI_ERROR));

    if (hIcon && hIconSm) {
        SendMessage(g_hMainWindow, WM_SETICON, ICON_BIG, (LPARAM)hIcon);
        SendMessage(g_hMainWindow, WM_SETICON, ICON_SMALL, (LPARAM)hIconSm);
    }

    // Create hacker fonts
    CreateHackerFont();

    // Create the menus
    CreateMenus(g_hMainWindow);

    // Create the controls
    CreateControls(g_hMainWindow);

    // Add ASCII art banner
    AddAsciiArt(g_hMainWindow);

    // Create matrix effect
    CreateMatrixEffect(g_hMainWindow);

    // Initialize image list for file icons
    InitializeImageList();

    // Set up the ListView
    InitializeListViewColumns(g_hListView);

    // Set the default path to My Documents
    wchar_t myDocumentsPath[MAX_PATH];
    SHGetFolderPath(NULL, CSIDL_PERSONAL, NULL, 0, myDocumentsPath);
    g_currentPath = myDocumentsPath;
    SetWindowText(g_hPathEdit, g_currentPath.c_str());

    // Show the window
    ShowWindow(g_hMainWindow, nCmdShow);
    UpdateWindow(g_hMainWindow);

    // Populate the list view
    PostMessage(g_hMainWindow, WM_POPULATE_LIST, 0, 0);

    return TRUE;
}

// Window procedure for the main window
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
    case WM_CREATE:
        InitializeMatrixEffect();
        SetTimer(hwnd, 1, 50, NULL); // Update matrix effect every 50ms
        return 0;

    case WM_CLOSE:
        // Cleanup resources before destroying the window
        KillTimer(hwnd, 1);
        if (g_hImageList) ImageList_Destroy(g_hImageList);
        if (g_hFont) DeleteObject(g_hFont);
        if (g_hHackerFont) DeleteObject(g_hHackerFont);
        if (g_hHackerFontSmall) DeleteObject(g_hHackerFontSmall);
        DestroyWindow(hwnd);
        return 0;

    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;

    case WM_TIMER:
        if (wParam == 1) {
            InvalidateRect(g_hMatrixEffect, NULL, TRUE);
            return 0;
        }
        if (wParam == IDC_STATUS_TIMER) {
            UpdateStatusBox();
            return 0;
        }
        break;

    case WM_PAINT:
        if (hwnd == g_hMatrixEffect) {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);
            RECT rc;
            GetClientRect(hwnd, &rc);
            DrawMatrixEffect(hdc, rc);
            EndPaint(hwnd, &ps);
            return 0;
        }
        break;

    case WM_SIZE:
        ResizeControls(hwnd);
        return 0;

    case WM_GETMINMAXINFO:
    {
        LPMINMAXINFO lpMMI = (LPMINMAXINFO)lParam;
        lpMMI->ptMinTrackSize.x = 640;  // Minimum width
        lpMMI->ptMinTrackSize.y = 480;  // Minimum height
        return 0;
    }

    case WM_CTLCOLOREDIT:
    case WM_CTLCOLORSTATIC:
    {
        HDC hdcStatic = (HDC)wParam;
        HWND hwndStatic = (HWND)lParam;
        
        if (hwndStatic == g_hCmdOutput || hwndStatic == g_hCmdEdit ||
            hwndStatic == g_hAsciiBanner || hwndStatic == g_hMatrixEffect) {
            SetTextColor(hdcStatic, RGB(0, 255, 0));
            SetBkColor(hdcStatic, RGB(0, 0, 0));
            static HBRUSH hBrush = CreateSolidBrush(RGB(0, 0, 0));
            return (LRESULT)hBrush;
        }
        break;
    }

    case WM_COMMAND:
        switch (LOWORD(wParam)) {
        case IDC_GO_BUTTON:
        {
            wchar_t path[MAX_PATH];
            GetWindowText(g_hPathEdit, path, MAX_PATH);
            NavigateToPath(path);
        }
        return 0;

        case IDC_UP_BUTTON:
            NavigateUp();
            return 0;

        case IDM_FILE_NEW:
            CreateNewFile();
            return 0;

        case IDM_FILE_COPY:
            CopySelectedFiles();
            return 0;

        case IDM_FILE_DELETE:
            DeleteSelectedFiles();
            return 0;

        case IDM_FILE_RENAME:
            RenameSelectedFile();
            return 0;

        case IDM_FILE_PROPERTIES:
        {
            int selectedIndex = ListView_GetNextItem(g_hListView, -1, LVNI_SELECTED);
            if (selectedIndex != -1) {
                wchar_t filename[MAX_PATH];
                LVITEM item = { 0 };
                item.iItem = selectedIndex;
                item.iSubItem = 0;
                item.mask = LVIF_TEXT;
                item.pszText = filename;
                item.cchTextMax = MAX_PATH - 1;  // Leave room for null terminator
                ListView_GetItem(g_hListView, &item);

                std::wstring fullPath = g_currentPath + L"\\" + filename;
                ShowFileProperties(fullPath);
            }
        }
        return 0;

        case IDM_FILE_EXIT:
            SendMessage(hwnd, WM_CLOSE, 0, 0);
            return 0;

        case IDM_VIEW_REFRESH:
            PopulateListView(g_hListView, g_currentPath);
            return 0;

        case IDC_CMD_ENTER:
            if (HIWORD(wParam) == BN_CLICKED) {
                ExecuteCommand();
                return 0;
            }
            break;

        case IDC_CMD_EDIT:
            if (HIWORD(wParam) == EN_UPDATE) {
                // Check for Enter key
                if (GetKeyState(VK_RETURN) & 0x8000) {
                    ExecuteCommand();
                    return 0;
                }
            }
            break;
        }
        break;

    case WM_NOTIFY: {
        LPNMHDR nmhdr = (LPNMHDR)lParam;
        if (nmhdr->hwndFrom == g_hListView) {
            switch (nmhdr->code) {
            case NM_CUSTOMDRAW: {
                LPNMLVCUSTOMDRAW lplvcd = (LPNMLVCUSTOMDRAW)lParam;
                switch (lplvcd->nmcd.dwDrawStage) {
                case CDDS_PREPAINT:
                    return CDRF_NOTIFYITEMDRAW;

                case CDDS_ITEMPREPAINT:
                    // Selected item: dark green background with bright green text
                    if (lplvcd->nmcd.uItemState & CDIS_SELECTED) {
                        lplvcd->clrText = RGB(0, 255, 0);      // Bright green text
                        lplvcd->clrTextBk = RGB(0, 64, 0);     // Dark green background
                    }
                    // Normal item: bright green text on black background
                    lplvcd->clrText = RGB(0, 255, 0);
                    lplvcd->clrTextBk = RGB(0, 0, 0);
                    return CDRF_NEWFONT;
                }
                return CDRF_DODEFAULT;
            }
                              break;

            case NM_DBLCLK: {
                // Double-click on list item
                int selectedIndex = ListView_GetNextItem(g_hListView, -1, LVNI_SELECTED);
                if (selectedIndex != -1) {
                    wchar_t filename[MAX_PATH] = { 0 };
                    LVITEM item = { 0 };
                    item.iItem = selectedIndex;
                    item.iSubItem = 0;
                    item.mask = LVIF_TEXT;
                    item.pszText = filename;
                    item.cchTextMax = MAX_PATH - 1;  // Leave room for null terminator
                    ListView_GetItem(g_hListView, &item);

                    if (wcscmp(filename, L"..") == 0) {
                        NavigateUp();
                    }
                    else {
                        std::wstring fullPath = g_currentPath + L"\\" + filename;
                        DWORD attributes = GetFileAttributes(fullPath.c_str());

                        if (attributes != INVALID_FILE_ATTRIBUTES && (attributes & FILE_ATTRIBUTE_DIRECTORY)) {
                            // It's a directory, navigate to it
                            NavigateToPath(fullPath);
                        }
                        else {
                            // It's a file, open it with default application
                            ShellExecute(hwnd, L"open", fullPath.c_str(), NULL, NULL, SW_SHOW);
                        }
                    }
                }
                break;
            }

            case LVN_ITEMCHANGED:
                // Update status bar when selection changes
                UpdateStatusBar(g_hListView);
                break;
            }
        }
    }
                  break;

    case WM_POPULATE_LIST:
        PopulateListView(g_hListView, g_currentPath);
        return 0;
    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

// Create the menus for the main window
void CreateMenus(_In_ HWND hwnd) {
    HMENU hMenu = CreateMenu();
    HMENU hFileMenu = CreatePopupMenu();
    HMENU hViewMenu = CreatePopupMenu();

    // File menu
    AppendMenu(hFileMenu, MF_STRING, IDM_FILE_NEW, L"&New File");
    AppendMenu(hFileMenu, MF_STRING, IDM_FILE_COPY, L"&Copy");
    AppendMenu(hFileMenu, MF_STRING, IDM_FILE_DELETE, L"&Delete");
    AppendMenu(hFileMenu, MF_STRING, IDM_FILE_RENAME, L"&Rename");
    AppendMenu(hFileMenu, MF_STRING, IDM_FILE_PROPERTIES, L"P&roperties");
    AppendMenu(hFileMenu, MF_SEPARATOR, 0, NULL);
    AppendMenu(hFileMenu, MF_STRING, IDM_FILE_EXIT, L"E&xit");

    // View menu
    AppendMenu(hViewMenu, MF_STRING, IDM_VIEW_REFRESH, L"&Refresh");

    // Add menus to main menu
    AppendMenu(hMenu, MF_POPUP, (UINT_PTR)hFileMenu, L"&File");
    AppendMenu(hMenu, MF_POPUP, (UINT_PTR)hViewMenu, L"&View");

    // Set the menu
    SetMenu(hwnd, hMenu);
}

// Create the controls for the main window
void CreateControls(_In_ HWND hwnd) {
    // Create ASCII art banner with adjusted height
    g_hAsciiBanner = CreateWindowEx(
        0,
        L"STATIC",
        ASCII_BANNER,
        WS_CHILD | WS_VISIBLE | SS_CENTER | SS_NOPREFIX,
        0, 0, 0, 0,  // Will be positioned in ResizeControls
        hwnd,
        NULL,
        g_hInstance,
        NULL
    );
    SendMessage(g_hAsciiBanner, WM_SETFONT, (WPARAM)g_hHackerFont, TRUE);

    // Create sidebar with improved styling
    g_hSideBar = CreateWindowEx(
        WS_EX_CLIENTEDGE,
        L"EDIT",
        L"",
        WS_CHILD | WS_VISIBLE | ES_MULTILINE | ES_READONLY | ES_LEFT,
        0, 0, 0, 0,
        hwnd,
        (HMENU)IDC_SIDEBAR,
        g_hInstance,
        NULL
    );
    
    // Apply custom window procedure to sidebar
    SetWindowSubclass(g_hSideBar, SideBarProc, 0, 0);
    SendMessage(g_hSideBar, WM_SETFONT, (WPARAM)g_hHackerFontSmall, TRUE);

    // Get start time for uptime calculation
    GetLocalTime(&g_startTime);

    // Create timer for status updates
    SetTimer(hwnd, IDC_STATUS_TIMER, 1000, NULL);

    // Create address bar controls with better spacing
    g_hPathEdit = CreateWindowEx(
        WS_EX_CLIENTEDGE,
        L"EDIT",
        L"",
        WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL,
        0, 0, 0, 0,
        hwnd,
        (HMENU)IDC_PATH_EDIT,
        g_hInstance,
        NULL
    );
    SendMessage(g_hPathEdit, WM_SETFONT, (WPARAM)g_hHackerFontSmall, TRUE);
    SetTextColor(GetDC(g_hPathEdit), RGB(0, 255, 0));
    SendMessage(g_hPathEdit, EM_SETBKGNDCOLOR, 0, (LPARAM)RGB(0, 0, 0));

    HWND hGoButton = CreateWindow(
        L"BUTTON",
        L"[GO]",
        WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
        0, 0, 0, 0,  // Will be resized later
        hwnd,
        (HMENU)IDC_GO_BUTTON,
        g_hInstance,
        NULL
    );
    SendMessage(hGoButton, WM_SETFONT, (WPARAM)g_hHackerFont, TRUE);

    HWND hUpButton = CreateWindow(
        L"BUTTON",
        L"[UP]",
        WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
        0, 0, 0, 0,  // Will be resized later
        hwnd,
        (HMENU)IDC_UP_BUTTON,
        g_hInstance,
        NULL
    );
    SendMessage(hUpButton, WM_SETFONT, (WPARAM)g_hHackerFont, TRUE);

    // Create list view with improved styling
    g_hListView = CreateWindowEx(
        WS_EX_CLIENTEDGE,
        WC_LISTVIEW,
        L"",
        WS_CHILD | WS_VISIBLE | LVS_REPORT | LVS_SHOWSELALWAYS | LVS_SHAREIMAGELISTS,
        0, 0, 0, 0,
        hwnd,
        (HMENU)IDC_LISTVIEW,
        g_hInstance,
        NULL
    );

    // Apply the hacker font to the list view
    SendMessage(g_hListView, WM_SETFONT, (WPARAM)g_hHackerFontSmall, TRUE);

    // Set ListView colors for better visibility
    ListView_SetTextColor(g_hListView, RGB(0, 255, 0));      // Bright green text
    ListView_SetBkColor(g_hListView, RGB(0, 0, 0));         // Black background
    ListView_SetTextBkColor(g_hListView, RGB(0, 0, 0));     // Black text background

    // Set extended styles for better appearance
    ListView_SetExtendedListViewStyle(g_hListView, 
        LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | LVS_EX_DOUBLEBUFFER);

    // Create status bar
    g_hStatusBar = CreateWindowEx(
        0,
        STATUSCLASSNAME,
        L"",
        WS_CHILD | WS_VISIBLE | SBARS_SIZEGRIP,
        0, 0, 0, 0,  // Will be resized automatically
        hwnd,
        (HMENU)IDC_STATUSBAR,
        g_hInstance,
        NULL
    );
    SendMessage(g_hStatusBar, WM_SETFONT, (WPARAM)g_hHackerFontSmall, TRUE);

    // Create command prompt with improved styling
    g_hCmdOutput = CreateWindowEx(
        WS_EX_CLIENTEDGE,
        L"EDIT",
        L"Microsoft Windows [Version 10.0.26100]\r\n"
        L"(c) Microsoft Corporation. All rights reserved.\r\n\r\n",
        WS_CHILD | WS_VISIBLE | WS_VSCROLL | ES_MULTILINE | ES_AUTOVSCROLL | ES_READONLY,
        0, 0, 0, 0,
        hwnd,
        (HMENU)IDC_CMD_OUTPUT,
        g_hInstance,
        NULL
    );

    // Try to set user's directory as default, fallback to C:\ if not found
    wchar_t userPath[MAX_PATH];
    if (SUCCEEDED(SHGetFolderPath(NULL, CSIDL_PROFILE, NULL, 0, userPath))) {
        if (SetCurrentDirectory(userPath)) {
            g_currentPath = userPath;
        } else {
            SetCurrentDirectory(L"C:\\");
            g_currentPath = L"C:\\";
        }
    } else {
        SetCurrentDirectory(L"C:\\");
        g_currentPath = L"C:\\";
    }
    
    // Add initial prompt with current path
    std::wstring initialPrompt = GetFormattedPrompt();
    int length = GetWindowTextLength(g_hCmdOutput);
    SendMessage(g_hCmdOutput, EM_SETSEL, length, length);
    SendMessage(g_hCmdOutput, EM_REPLACESEL, FALSE, (LPARAM)initialPrompt.c_str());

    g_hCmdEdit = CreateWindowEx(
        WS_EX_CLIENTEDGE,
        L"EDIT",
        L"",
        WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL | ES_MULTILINE,
        0, 0, 0, 0,
        hwnd,
        (HMENU)IDC_CMD_EDIT,
        g_hInstance,
        NULL
    );

    // Create Enter button for command execution
    HWND hEnterButton = CreateWindow(
        L"BUTTON",
        L"[ENTER]",
        WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
        0, 0, 0, 0,
        hwnd,
        (HMENU)IDC_CMD_ENTER,
        g_hInstance,
        NULL
    );
    SendMessage(hEnterButton, WM_SETFONT, (WPARAM)g_hHackerFontSmall, TRUE);

    // Set up command prompt subclassing for Enter key handling
    SetWindowSubclass(g_hCmdEdit, CmdEditSubclassProc, 0, 0);

    // Apply hacker style to command interface
    SendMessage(g_hCmdOutput, WM_SETFONT, (WPARAM)g_hHackerFontSmall, TRUE);
    SendMessage(g_hCmdEdit, WM_SETFONT, (WPARAM)g_hHackerFontSmall, TRUE);

    // Set colors for command interface
    SendMessage(g_hCmdOutput, EM_SETBKGNDCOLOR, 0, (LPARAM)RGB(0, 0, 0));
    SendMessage(g_hCmdEdit, EM_SETBKGNDCOLOR, 0, (LPARAM)RGB(0, 0, 0));

    // Initial resize
    ResizeControls(hwnd);
}

// Resize controls when the window is resized
void ResizeControls(_In_ HWND hwnd) {
    // Constants for sizing with better spacing
    const int sidebarWidth = 200;    // Width for the sidebar
    const int statusHeight = 20;
    const int addressBarHeight = 25;
    const int buttonWidth = 60;
    const int margin = 5;
    const int cmdPromptHeight = 150;
    const int cmdEditHeight = 25;
    const int bannerHeight = 100;  // Height for the ASCII art banner

    RECT rcClient;
    GetClientRect(hwnd, &rcClient);

    // Position ASCII art banner at the top
    MoveWindow(g_hAsciiBanner,
        margin,
        margin,
        rcClient.right - sidebarWidth - 3 * margin,
        bannerHeight,
        TRUE);

    // Position sidebar on the right
    MoveWindow(g_hSideBar,
        rcClient.right - sidebarWidth - margin,
        margin,
        sidebarWidth,
        rcClient.bottom - 2 * margin,
        TRUE);

    // Position address bar controls below the banner
    int addressBarTop = margin + bannerHeight + margin;
    
    MoveWindow(g_hPathEdit,
        margin,
        addressBarTop,
        rcClient.right - sidebarWidth - 4 * margin - 2 * buttonWidth,
        addressBarHeight,
        TRUE);

    MoveWindow(GetDlgItem(hwnd, IDC_GO_BUTTON),
        rcClient.right - sidebarWidth - 2 * buttonWidth - 3 * margin,
        addressBarTop,
        buttonWidth,
        addressBarHeight,
        TRUE);

    MoveWindow(GetDlgItem(hwnd, IDC_UP_BUTTON),
        rcClient.right - sidebarWidth - buttonWidth - 2 * margin,
        addressBarTop,
        buttonWidth,
        addressBarHeight,
        TRUE);

    // Position list view below address bar
    int listViewTop = addressBarTop + addressBarHeight + margin;
    int listViewHeight = rcClient.bottom - listViewTop - cmdPromptHeight - statusHeight - margin;

    MoveWindow(g_hListView,
        margin,
        listViewTop,
        rcClient.right - sidebarWidth - 3 * margin,
        listViewHeight,
        TRUE);

    // Position command prompt interface at the bottom
    int cmdTop = rcClient.bottom - cmdPromptHeight - statusHeight;
    
    MoveWindow(g_hCmdOutput,
        margin,
        cmdTop,
        rcClient.right - sidebarWidth - 3 * margin,
        cmdPromptHeight - cmdEditHeight - margin,
        TRUE);

    // Position command input and enter button
    int cmdEditTop = rcClient.bottom - statusHeight - cmdEditHeight - margin;
    
    MoveWindow(g_hCmdEdit,
        margin,
        cmdEditTop,
        rcClient.right - sidebarWidth - buttonWidth - 4 * margin,
        cmdEditHeight,
        TRUE);

    MoveWindow(GetDlgItem(hwnd, IDC_CMD_ENTER),
        rcClient.right - sidebarWidth - buttonWidth - 2 * margin,
        cmdEditTop,
        buttonWidth,
        cmdEditHeight,
        TRUE);

    // Position status bar
    MoveWindow(g_hStatusBar,
        0,
        rcClient.bottom - statusHeight,
        rcClient.right - sidebarWidth - 2 * margin,
        statusHeight,
        TRUE);
}

// Initialize the columns for the list view
void InitializeListViewColumns(_In_ HWND hListView) {
    LVCOLUMN lvc = { 0 };
    lvc.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;

    // Name column - wider for better visibility
    lvc.iSubItem = 0;
    lvc.pszText = const_cast<LPWSTR>(L"Name");
    lvc.cx = 300;
    lvc.fmt = LVCFMT_LEFT;
    ListView_InsertColumn(hListView, 0, &lvc);

    // Size column - right-aligned with fixed width
    lvc.iSubItem = 1;
    lvc.pszText = const_cast<LPWSTR>(L"Size");
    lvc.cx = 100;
    lvc.fmt = LVCFMT_RIGHT;
    ListView_InsertColumn(hListView, 1, &lvc);

    // Type column - appropriate width for content
    lvc.iSubItem = 2;
    lvc.pszText = const_cast<LPWSTR>(L"Type");
    lvc.cx = 150;
    lvc.fmt = LVCFMT_LEFT;
    ListView_InsertColumn(hListView, 2, &lvc);

    // Date modified column - fixed width for consistency
    lvc.iSubItem = 3;
    lvc.pszText = const_cast<LPWSTR>(L"Date Modified");
    lvc.cx = 150;
    lvc.fmt = LVCFMT_LEFT;
    ListView_InsertColumn(hListView, 3, &lvc);
}

// Initialize the image list for file icons
void InitializeImageList() {
    // Create image list
    g_hImageList = ImageList_Create(16, 16, ILC_COLOR32 | ILC_MASK, 10, 10);

    // Add default file and folder icons
    SHFILEINFO sfi = {0};

    // Get folder icon
    if (SHGetFileInfo(L"C:\\", 0, &sfi, sizeof(SHFILEINFO), SHGFI_SYSICONINDEX | SHGFI_SMALLICON)) {
        if (sfi.hIcon != NULL) {  // Add null check
            int folderIndex = ImageList_AddIcon(g_hImageList, sfi.hIcon);
            if (sfi.hIcon != NULL) {  // Add null check
                DestroyIcon(sfi.hIcon);
            }
        }
    }

    // Get file icon
    if (SHGetFileInfo(L"C:\\dummy.txt", 0, &sfi, sizeof(SHFILEINFO), SHGFI_SYSICONINDEX | SHGFI_SMALLICON | SHGFI_USEFILEATTRIBUTES)) {
        if (sfi.hIcon != NULL) {  // Add null check
            int fileIndex = ImageList_AddIcon(g_hImageList, sfi.hIcon);
            if (sfi.hIcon != NULL) {  // Add null check
                DestroyIcon(sfi.hIcon);
            }
        }
    }

    // Set the image list to the list view
    ListView_SetImageList(g_hListView, g_hImageList, LVSIL_SMALL);
}

// Get icon index for a file
int GetIconIndex(_In_ const std::wstring& filePath, _In_ bool isDirectory) {
    static std::vector<std::wstring> cachedExtensions;
    static std::vector<int> cachedIconIndices;

    if (isDirectory) {
        return 0; // Folder icon is at index 0
    }

    // Extract file extension
    std::wstring fileExt = PathFindExtension(filePath.c_str());

    // Check if we already have this extension cached
    for (size_t i = 0; i < cachedExtensions.size(); i++) {
        if (_wcsicmp(cachedExtensions[i].c_str(), fileExt.c_str()) == 0) {
            return cachedIconIndices[i];
        }
    }

    // Not cached, get the icon
    SHFILEINFO sfi = { 0 };
    SHGetFileInfo(filePath.c_str(), 0, &sfi, sizeof(SHFILEINFO), SHGFI_SYSICONINDEX | SHGFI_SMALLICON);

    // Add to cache and return
    int iconIndex = 0;
    if (sfi.hIcon != NULL) {  // Add null check
        iconIndex = ImageList_AddIcon(g_hImageList, sfi.hIcon);
        if (sfi.hIcon != NULL) {  // Add null check
            DestroyIcon(sfi.hIcon);
        }
    }

    cachedExtensions.push_back(fileExt);
    cachedIconIndices.push_back(iconIndex);

    return iconIndex;
}

// Populate the list view with files and folders
void PopulateListView(_In_ HWND hListView, _In_ const std::wstring& path) {
    // Clear the list view
    ListView_DeleteAllItems(hListView);

    // Initialize itemIndex at the start of the function
    int itemIndex = 0;

    // Add parent directory entry if not at root
    if (path.length() > 3) {  // Skip for drives like "C:\"
        LVITEM lvi = { 0 };
        lvi.mask = LVIF_TEXT | LVIF_IMAGE;
        lvi.iItem = itemIndex++;
        lvi.iSubItem = 0;
        lvi.pszText = const_cast<LPWSTR>(L"..");
        lvi.iImage = 0;  // Folder icon
        ListView_InsertItem(hListView, &lvi);
    }

    // Set up the search path
    std::wstring searchPath = path;
    if (searchPath.back() != L'\\') {
        searchPath += L"\\";
    }
    searchPath += L"*";

    // Find files and folders
    WIN32_FIND_DATA findData;
    HANDLE hFind = FindFirstFile(searchPath.c_str(), &findData);

    if (hFind != INVALID_HANDLE_VALUE) {
        do {
            // Skip "." and ".." directories
            if (wcscmp(findData.cFileName, L".") == 0)
                continue;

            if (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
                // Skip ".." as we add it manually
                if (wcscmp(findData.cFileName, L"..") != 0) {
                    LVITEM lvi = { 0 };
                    lvi.mask = LVIF_TEXT | LVIF_IMAGE;
                    lvi.iItem = itemIndex++;
                    lvi.iSubItem = 0;
                    lvi.pszText = const_cast<LPWSTR>(findData.cFileName);
                    lvi.cchTextMax = static_cast<int>(wcslen(findData.cFileName));
                    lvi.iImage = 0;  // Folder icon
                    int index = ListView_InsertItem(hListView, &lvi);

                    // Add folder type
                    lvi.mask = LVIF_TEXT;
                    lvi.iItem = index;
                    lvi.iSubItem = 1;
                    lvi.pszText = const_cast<LPWSTR>(L"");
                    ListView_SetItem(hListView, &lvi);

                    lvi.iSubItem = 2;
                    lvi.pszText = const_cast<LPWSTR>(L"File Folder");
                    ListView_SetItem(hListView, &lvi);

                    // Add date modified
                    FILETIME localFileTime;
                    SYSTEMTIME systemTime;
                    wchar_t dateStr[64] = { 0 };

                    FileTimeToLocalFileTime(&findData.ftLastWriteTime, &localFileTime);
                    FileTimeToSystemTime(&localFileTime, &systemTime);

                    GetDateFormat(LOCALE_USER_DEFAULT, 0, &systemTime,
                        L"MM/dd/yyyy", dateStr, _countof(dateStr));

                    lvi.iSubItem = 3;
                    lvi.pszText = dateStr;
                    ListView_SetItem(hListView, &lvi);
                }
            }
            else {
                LVITEM lvi = { 0 };
                lvi.mask = LVIF_TEXT | LVIF_IMAGE;
                lvi.iItem = itemIndex++;
                lvi.iSubItem = 0;
                lvi.pszText = const_cast<LPWSTR>(findData.cFileName);

                std::wstring fullPath = path + L"\\" + findData.cFileName;
                lvi.iImage = GetIconIndex(fullPath, FALSE);
                ListView_InsertItem(hListView, &lvi);

                // Add file size
                wchar_t sizeStr[32] = { 0 };
                ULARGE_INTEGER fileSize;
                fileSize.LowPart = findData.nFileSizeLow;
                fileSize.HighPart = findData.nFileSizeHigh;

                if (fileSize.QuadPart < 1024) {
                    StringCchPrintf(sizeStr, 32, L"%llu bytes", fileSize.QuadPart);
                }
                else if (fileSize.QuadPart < 1024 * 1024) {
                    StringCchPrintf(sizeStr, 32, L"%.2f KB", (double)fileSize.QuadPart / 1024.0);
                }
                else if (fileSize.QuadPart < 1024 * 1024 * 1024) {
                    StringCchPrintf(sizeStr, 32, L"%.2f MB", (double)fileSize.QuadPart / (1024.0 * 1024.0));
                }
                else {
                    StringCchPrintf(sizeStr, 32, L"%.2f GB", (double)fileSize.QuadPart / (1024.0 * 1024.0 * 1024.0));
                }

                lvi.mask = LVIF_TEXT;
                lvi.iSubItem = 1;
                lvi.pszText = sizeStr;
                ListView_SetItem(hListView, &lvi);

                // Add file type
                std::wstring fileExt = PathFindExtension(findData.cFileName);
                wchar_t fileType[MAX_PATH] = L"File";

                if (!fileExt.empty()) {
                    SHFILEINFO sfi = { 0 };
                    SHGetFileInfo(fileExt.c_str(), FILE_ATTRIBUTE_NORMAL, &sfi, sizeof(SHFILEINFO),
                        SHGFI_TYPENAME | SHGFI_USEFILEATTRIBUTES);

                    if (sfi.szTypeName[0] != L'\0') {
                        wcscpy_s(fileType, MAX_PATH, sfi.szTypeName);
                    }
                    else {
                        StringCchPrintf(fileType, MAX_PATH, L"%s File", fileExt.c_str() + 1);
                    }
                }

                lvi.iSubItem = 2;
                lvi.pszText = fileType;
                ListView_SetItem(hListView, &lvi);

                // Add date modified
                FILETIME localFileTime;
                SYSTEMTIME systemTime;
                wchar_t dateStr[64] = { 0 };

                FileTimeToLocalFileTime(&findData.ftLastWriteTime, &localFileTime);
                FileTimeToSystemTime(&localFileTime, &systemTime);

                GetDateFormat(LOCALE_USER_DEFAULT, 0, &systemTime,
                    L"MM/dd/yyyy", dateStr, _countof(dateStr));

                lvi.iSubItem = 3;
                lvi.pszText = dateStr;
                ListView_SetItem(hListView, &lvi);
            }
        } while (FindNextFile(hFind, &findData));

        FindClose(hFind);
    }

    // Update status bar
    UpdateStatusBar(hListView);
}

// Navigate to a new path
void NavigateToPath(_In_ const std::wstring& path) {
    // Check if the path exists and is a directory
    DWORD attributes = GetFileAttributes(path.c_str());
    if (attributes != INVALID_FILE_ATTRIBUTES && (attributes & FILE_ATTRIBUTE_DIRECTORY)) {
        g_currentPath = path;
        SetWindowText(g_hPathEdit, g_currentPath.c_str());
        PopulateListView(g_hListView, g_currentPath);
        PlaySuccessSound();
    }
    else {
        MessageBox(g_hMainWindow, L"Invalid directory path!", L"Error", MB_ICONERROR | MB_OK);
        PlayErrorSound();
    }
}

// Navigate up one directory
void NavigateUp() {
    if (g_currentPath.length() <= 3) {  // Don't go up from a root directory (e.g., "C:\")
        return;
    }

    // Find the last backslash
    size_t pos = g_currentPath.find_last_of(L'\\');
    if (pos != std::wstring::npos && pos > 2) {  // Make sure we don't remove the drive letter (e.g., "C:\")
        std::wstring newPath = g_currentPath.substr(0, pos);
        NavigateToPath(newPath);
    }
}

// Show file properties dialog
void ShowFileProperties(_In_ const std::wstring& filePath) {
    SHELLEXECUTEINFO sei = { 0 };
    sei.cbSize = sizeof(SHELLEXECUTEINFO);
    sei.lpFile = filePath.c_str();
    sei.nShow = SW_SHOW;
    sei.fMask = SEE_MASK_INVOKEIDLIST;
    sei.lpVerb = L"properties";

    ShellExecuteEx(&sei);
}

// Delete selected files
void DeleteSelectedFiles() {
    int selectedCount = ListView_GetSelectedCount(g_hListView);
    if (selectedCount == 0) {
        return;
    }

    // Confirm deletion
    wchar_t message[256];
    StringCchPrintf(message, 256, L"Are you sure you want to delete the selected %d item(s)?", selectedCount);
    int result = MessageBox(g_hMainWindow, message, L"Confirm Delete", MB_YESNO | MB_ICONQUESTION);

    if (result != IDYES) {
        PlayErrorSound();
        return;
    }

    // Get selected items and delete them
    std::vector<std::wstring> filesToDelete;
    int itemIndex = -1;

    while ((itemIndex = ListView_GetNextItem(g_hListView, itemIndex, LVNI_SELECTED)) != -1) {
        wchar_t filename[MAX_PATH] = { 0 };  // Zero-initialize the buffer
        LVITEM item = { 0 };
        item.iItem = itemIndex;
        item.iSubItem = 0;
        item.mask = LVIF_TEXT;
        item.pszText = filename;
        item.cchTextMax = MAX_PATH;
        if (ListView_GetItem(g_hListView, &item)) {
            // Skip ".." entry
            if (wcscmp(filename, L"..") == 0) {
                continue;
            }
            std::wstring fullPath = g_currentPath + L"\\" + filename;
            filesToDelete.push_back(fullPath);
        }
    }

    // Delete the files
    bool anyError = false;
    for (const auto& filePath : filesToDelete) {
        DWORD attr = GetFileAttributes(filePath.c_str());
        if (attr & FILE_ATTRIBUTE_DIRECTORY) {
            // It's a directory, use RemoveDirectory
            if (!RemoveDirectory(filePath.c_str())) {
                anyError = true;
            }
        }
        else {
            // It's a file, use DeleteFile
            if (!DeleteFile(filePath.c_str())) {
                anyError = true;
            }
        }
    }

    if (anyError) {
        MessageBox(g_hMainWindow, L"Some items could not be deleted.", L"Error", MB_ICONERROR | MB_OK);
        PlayErrorSound();
    }
    else {
        PlaySuccessSound();
    }

    // Refresh the view
    PopulateListView(g_hListView, g_currentPath);
}

// Copy selected files
void CopySelectedFiles() {
    int selectedCount = ListView_GetSelectedCount(g_hListView);
    if (selectedCount == 0) {
        return;
    }

    // Get all selected filenames
    std::vector<std::wstring> filesToCopy;
    int itemIndex = -1;

    while ((itemIndex = ListView_GetNextItem(g_hListView, itemIndex, LVNI_SELECTED)) != -1) {
        wchar_t filename[MAX_PATH] = { 0 };  // Zero-initialize the buffer
        LVITEM item = { 0 };
        item.iItem = itemIndex;
        item.iSubItem = 0;
        item.mask = LVIF_TEXT;
        item.pszText = filename;
        item.cchTextMax = MAX_PATH;
        if (ListView_GetItem(g_hListView, &item)) {
            // Skip ".." entry
            if (wcscmp(filename, L"..") == 0) {
                continue;
            }
            std::wstring fullPath = g_currentPath + L"\\" + filename;
            filesToCopy.push_back(fullPath);
        }
    }

    if (filesToCopy.empty()) {
        return;
    }

    // Calculate needed buffer size
    size_t bufferSize = 0;
    for (const auto& filePath : filesToCopy) {
        bufferSize += filePath.length() + 1;  // +1 for null terminator
    }
    bufferSize += 1;  // Extra null terminator for the end of the list

    // Allocate buffer and fill it with the file paths
    HANDLE hGlobal = GlobalAlloc(GMEM_MOVEABLE, (bufferSize + 1) * sizeof(wchar_t));
    if (!hGlobal) {
        return;
    }

    wchar_t* buffer = (wchar_t*)GlobalLock(hGlobal);
    if (!buffer) {
        GlobalFree(hGlobal);
        return;
    }

    wchar_t* pBuffer = buffer;
    for (const auto& filePath : filesToCopy) {
        StringCchCopy(pBuffer, filePath.length() + 1, filePath.c_str());
        pBuffer += filePath.length() + 1;
    }
    *pBuffer = L'\0';  // Double null terminator

    GlobalUnlock(hGlobal);

    // Open clipboard and copy the files
    if (OpenClipboard(g_hMainWindow)) {
        EmptyClipboard();
        SetClipboardData(CF_HDROP, hGlobal);
        CloseClipboard();
    }
    else {
        GlobalFree(hGlobal);
    }
}

// Rename selected file
void RenameSelectedFile() {
    int selectedIndex = ListView_GetNextItem(g_hListView, -1, LVNI_SELECTED);
    if (selectedIndex == -1 || ListView_GetSelectedCount(g_hListView) != 1) {
        MessageBox(g_hMainWindow, L"Please select a single file to rename.", L"Rename", MB_OK | MB_ICONINFORMATION);
        return;
    }

    wchar_t oldFilename[MAX_PATH] = { 0 };  // Zero-initialize the buffer
    LVITEM item = { 0 };
    item.iItem = selectedIndex;
    item.iSubItem = 0;
    item.mask = LVIF_TEXT;
    item.pszText = oldFilename;
    item.cchTextMax = MAX_PATH;
    if (!ListView_GetItem(g_hListView, &item)) {
        return;
    }

    // Skip ".." entry
    if (wcscmp(oldFilename, L"..") == 0) {
        return;
    }

    // Show input dialog for new name
    wchar_t newFilename[MAX_PATH];
    wcscpy_s(newFilename, MAX_PATH, oldFilename);

    // Simple implementation using InputBox - in a real app, use a custom dialog
    if (MessageBox(g_hMainWindow, L"Rename file?", L"Rename", MB_YESNO | MB_ICONQUESTION) == IDYES) {
        // For simplicity, we're just using a message box as a placeholder
        // In a real app, you would create a dialog with an edit control

        std::wstring oldPath = g_currentPath + L"\\" + oldFilename;
        std::wstring newPath = g_currentPath + L"\\" + newFilename;

        if (MoveFile(oldPath.c_str(), newPath.c_str())) {
            PopulateListView(g_hListView, g_currentPath);
        }
        else {
            MessageBox(g_hMainWindow, L"Failed to rename file.", L"Error", MB_ICONERROR | MB_OK);
        }
    }
}

// Create a new file
void CreateNewFile() {
    // Show input dialog for new name
    wchar_t newFilename[MAX_PATH] = L"New File.txt";

    // Simple implementation using MessageBox - in a real app, use a custom dialog
    if (MessageBox(g_hMainWindow, L"Create new file?", L"New File", MB_YESNO | MB_ICONQUESTION) == IDYES) {
        // For simplicity, we're just using a message box as a placeholder
        // In a real app, you would create a dialog with an edit control

        std::wstring newPath = g_currentPath + L"\\" + newFilename;

        // Create the file
        HANDLE hFile = CreateFile(
            newPath.c_str(),
            GENERIC_WRITE,
            0,
            NULL,
            CREATE_NEW,
            FILE_ATTRIBUTE_NORMAL,
            NULL
        );

        if (hFile != INVALID_HANDLE_VALUE) {
            CloseHandle(hFile);
            PopulateListView(g_hListView, g_currentPath);
        }
        else {
            MessageBox(g_hMainWindow, L"Failed to create file.", L"Error", MB_ICONERROR | MB_OK);
        }
    }
}

// Update status bar with selection information
void UpdateStatusBar(_In_ HWND hListView) {
    int totalItems = ListView_GetItemCount(hListView);
    int selectedItems = ListView_GetSelectedCount(hListView);

    wchar_t statusText[256];
    if (selectedItems == 0) {
        StringCchPrintf(statusText, 256, L"%d item(s)", totalItems);
    }
    else {
        StringCchPrintf(statusText, 256, L"%d item(s) selected", selectedItems);
    }

    SendMessage(g_hStatusBar, SB_SETTEXT, 0, (LPARAM)statusText);
}

void CreateHackerFont() {
    // Create a larger font for the banner
    g_hHackerFont = CreateFont(
        20,                 // Slightly smaller height
        0,                  // Width (0 = auto)
        0,                  // Escapement
        0,                  // Orientation
        FW_BOLD,           // Weight
        FALSE,             // Italic
        FALSE,             // Underline
        FALSE,             // StrikeOut
        ANSI_CHARSET,      // CharSet
        OUT_DEFAULT_PRECIS,
        CLIP_DEFAULT_PRECIS,
        ANTIALIASED_QUALITY,
        FIXED_PITCH | FF_MODERN,
        L"Terminal"        // Changed to Terminal font for better ASCII art display
    );

    // Create a smaller font for the list view
    g_hHackerFontSmall = CreateFont(
        16,                 // Height
        0,                  // Width (0 = auto)
        0,                  // Escapement
        0,                  // Orientation
        FW_NORMAL,         // Weight
        FALSE,             // Italic
        FALSE,             // Underline
        FALSE,             // StrikeOut
        ANSI_CHARSET,      // CharSet
        OUT_DEFAULT_PRECIS,
        CLIP_DEFAULT_PRECIS,
        ANTIALIASED_QUALITY,
        FIXED_PITCH | FF_MODERN,
        L"Terminal"        // Changed to Terminal font for consistency
    );
}

void AddAsciiArt(_In_ HWND hwnd) {
    // Simple keyboard-symbol ASCII art
    const wchar_t* SIMPLE_ASCII_BANNER = 
        L"    /\\    /\\    /\\    /\\    /\\    /\\    /\\    \n"
        L"   /  \\  /  \\  /  \\  /  \\  /  \\  /  \\  /  \\   \n"
        L"  <[dynamic file handler]> \n"
        L"  \\____/\\____/\\____/\\____/\\____/\\____/\\____/  \n";

    g_hAsciiBanner = CreateWindowEx(
        0,
        L"STATIC",
        SIMPLE_ASCII_BANNER,
        WS_CHILD | WS_VISIBLE | SS_CENTER | SS_NOPREFIX,
        0, 0, 800, 100,  // Increased height for the new banner
        hwnd,
        NULL,
        g_hInstance,
        NULL
    );

    // Use the hacker font for the banner
    SendMessage(g_hAsciiBanner, WM_SETFONT, (WPARAM)g_hHackerFont, TRUE);
}

void CreateMatrixEffect(_In_ HWND hwnd) {
    g_hMatrixEffect = CreateWindow(
        L"STATIC",
        L"",
        WS_CHILD | WS_VISIBLE | SS_CENTER,
        0, 0, 800, 20,  // Reduced height
        hwnd,
        NULL,
        g_hInstance,
        NULL
    );
    SendMessage(g_hMatrixEffect, WM_SETFONT, (WPARAM)g_hHackerFontSmall, TRUE);
}

void DrawMatrixEffect(_In_ HDC hdc, _In_ RECT rc) {
    static int frame = 0;
    frame++;

    // Update matrix positions
    for (int i = 0; i < MATRIX_COLUMNS; i++) {
        if (frame % matrixSpeeds[i] == 0) {
            matrixPositions[i]++;
            if (matrixPositions[i] >= MATRIX_ROWS) {
                matrixPositions[i] = 0;
                // Randomly change some characters
                for (int j = 0; j < MATRIX_ROWS; j++) {
                    if (rand() % 10 == 0) {
                        matrixChars[i][j] = L'0' + rand() % 2;
                    }
                }
            }
        }
    }

    // Draw matrix effect
    int charWidth = rc.right / MATRIX_COLUMNS;
    int charHeight = rc.bottom / MATRIX_ROWS;

    for (int i = 0; i < MATRIX_COLUMNS; i++) {
        for (int j = 0; j < MATRIX_ROWS; j++) {
            int pos = (matrixPositions[i] + j) % MATRIX_ROWS;
            wchar_t ch = matrixChars[i][pos];
            
            // Fade out effect
            int alpha = 255 - (j * 25);
            if (alpha < 0) alpha = 0;
            
            SetTextColor(hdc, RGB(0, alpha, 0));
            TextOut(hdc, i * charWidth, j * charHeight, &ch, 1);
        }
    }
}

void InitializeMatrixEffect() {
    // Initialize matrix characters
    for (int i = 0; i < MATRIX_COLUMNS; i++) {
        matrixSpeeds[i] = rand() % 3 + 1;
        matrixPositions[i] = -rand() % MATRIX_ROWS;
        for (int j = 0; j < MATRIX_ROWS; j++) {
            matrixChars[i][j] = L'0' + rand() % 2;
        }
    }
}

// Helper function to set text color for a window
void SetWindowTextColor(_In_ HWND hwnd, _In_ COLORREF color) {
    HDC hdc = GetDC(hwnd);
    SetTextColor(hdc, color);
    ReleaseDC(hwnd, hdc);
}

// Sound effects
void PlayKeySound() {
    PlaySound(L"SystemExclamation", NULL, SND_ASYNC | SND_ALIAS | SND_NODEFAULT);
}

void PlayErrorSound() {
    PlaySound(L"SystemHand", NULL, SND_ASYNC | SND_ALIAS | SND_NODEFAULT);
}

void PlaySuccessSound() {
    PlaySound(L"SystemAsterisk", NULL, SND_ASYNC | SND_ALIAS | SND_NODEFAULT);
}

// Add this new function for command prompt subclassing
LRESULT CALLBACK CmdEditSubclassProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData) {
    if (uMsg == WM_KEYDOWN && wParam == VK_RETURN) {
        ExecuteCommand();
        return 0;
    }
    return DefSubclassProc(hwnd, uMsg, wParam, lParam);
}

// Modify the ExecuteCommand function to not take a parameter
void ExecuteCommand() {
    wchar_t cmdBuffer[1024] = { 0 };
    GetWindowText(g_hCmdEdit, cmdBuffer, 1024);
    SetWindowText(g_hCmdEdit, L"");

    std::wstring cmd(cmdBuffer);
    while (!cmd.empty() && (cmd.back() == L'\r' || cmd.back() == L'\n')) {
        cmd.pop_back();
    }

    if (cmd.empty()) {
        int length = GetWindowTextLength(g_hCmdOutput);
        SendMessage(g_hCmdOutput, EM_SETSEL, length, length);
        std::wstring prompt = GetFormattedPrompt();
        SendMessage(g_hCmdOutput, EM_REPLACESEL, FALSE, (LPARAM)prompt.c_str());
        return;
    }

    // Handle internal commands
    if (_wcsicmp(cmd.c_str(), L"cls") == 0) {
        SetWindowText(g_hCmdOutput, L"");
        std::wstring prompt = GetFormattedPrompt();
        SendMessage(g_hCmdOutput, EM_REPLACESEL, FALSE, (LPARAM)prompt.c_str());
        return;
    }
    else if (_wcsicmp(cmd.c_str(), L"exit") == 0) {
        PostMessage(g_hMainWindow, WM_CLOSE, 0, 0);
        return;
    }

    // Add command to output with current directory
    std::wstring prompt = GetFormattedPrompt();
    std::wstring cmdLine;
    cmdLine.append(prompt);
    cmdLine.append(cmd);
    cmdLine.append(L"\r\n");
    
    int length = GetWindowTextLength(g_hCmdOutput);
    SendMessage(g_hCmdOutput, EM_SETSEL, length, length);
    SendMessage(g_hCmdOutput, EM_REPLACESEL, FALSE, (LPARAM)cmdLine.c_str());

    // Create pipes for command output
    SECURITY_ATTRIBUTES sa = { sizeof(SECURITY_ATTRIBUTES), NULL, TRUE };
    HANDLE hReadPipe, hWritePipe;
    
    if (!CreatePipe(&hReadPipe, &hWritePipe, &sa, 0)) {
        SendMessage(g_hCmdOutput, EM_REPLACESEL, FALSE, (LPARAM)L"Error: Failed to create pipe\r\n");
        return;
    }

    // Set up process info
    STARTUPINFO si = { sizeof(STARTUPINFO) };
    PROCESS_INFORMATION pi = { 0 };
    si.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;
    si.wShowWindow = SW_HIDE;
    si.hStdOutput = hWritePipe;
    si.hStdError = hWritePipe;
    si.hStdInput = GetStdHandle(STD_INPUT_HANDLE);

    // Build the command line
    std::wstring fullCmd;
    if (cmd.find(L"python") == 0 || cmd.find(L"py ") == 0) {
        // For Python commands, use python.exe directly
        wchar_t pythonPath[MAX_PATH];
        if (SearchPath(NULL, L"python.exe", NULL, MAX_PATH, pythonPath, NULL)) {
            // Check if it's just 'python' command (interactive mode)
            if (cmd == L"python" || cmd == L"py") {
                // For interactive Python, launch a new command prompt window
                std::wstring launchCmd = L"start cmd /k \"";
                launchCmd += pythonPath;
                launchCmd += L"\"";
                
                _wsystem(launchCmd.c_str());
                
                // Add new prompt
                length = GetWindowTextLength(g_hCmdOutput);
                SendMessage(g_hCmdOutput, EM_SETSEL, length, length);
                prompt = GetFormattedPrompt();
                SendMessage(g_hCmdOutput, EM_REPLACESEL, FALSE, (LPARAM)(L"\r\n" + prompt).c_str());
                return;
            }
            
            // Extract the Python script path and arguments
            size_t spacePos = cmd.find(L" ");
            if (spacePos != std::wstring::npos) {
                std::wstring scriptPath = cmd.substr(spacePos + 1);
                // If path is not quoted and contains spaces, add quotes
                if (scriptPath[0] != L'"' && scriptPath.find(L" ") != std::wstring::npos) {
                    scriptPath = L"\"" + scriptPath + L"\"";
                }
                fullCmd = std::wstring(L"\"") + pythonPath + L"\" " + scriptPath;
            } else {
                fullCmd = std::wstring(L"\"") + pythonPath + L"\"";
            }
        } else {
            SendMessage(g_hCmdOutput, EM_REPLACESEL, FALSE, (LPARAM)L"Error: Python not found in PATH\r\n");
            CloseHandle(hReadPipe);
            CloseHandle(hWritePipe);
            return;
        }
    } else {
        // For other commands, use cmd.exe
        fullCmd = L"/C " + cmd;
    }

    // Create the process
    BOOL processCreated = CreateProcess(
        cmd.find(L"python") == 0 || cmd.find(L"py ") == 0 ? NULL : L"C:\\Windows\\System32\\cmd.exe",
        &fullCmd[0],
        NULL,
        NULL,
        TRUE,
        CREATE_NO_WINDOW,
        NULL,
        g_currentPath.c_str(),
        &si,
        &pi
    );

    if (processCreated) {
        CloseHandle(hWritePipe);

        // Read output
        std::string output;
        char buffer[4096];
        DWORD bytesRead;
        
        while (ReadFile(hReadPipe, buffer, sizeof(buffer) - 1, &bytesRead, NULL) && bytesRead > 0) {
            buffer[bytesRead] = 0;
            output += buffer;
        }

        // Convert output to wide string and handle special characters
        if (!output.empty()) {
            int wlen = MultiByteToWideChar(CP_UTF8, 0, output.c_str(), -1, NULL, 0);
            if (wlen > 0) {
                std::vector<wchar_t> woutput(wlen);
                MultiByteToWideChar(CP_UTF8, 0, output.c_str(), -1, woutput.data(), wlen);
                
                // Replace any \n with \r\n for proper display
                std::wstring formattedOutput = woutput.data();
                size_t pos = 0;
                while ((pos = formattedOutput.find(L"\n", pos)) != std::wstring::npos) {
                    if (pos == 0 || formattedOutput[pos - 1] != L'\r') {
                        formattedOutput.replace(pos, 1, L"\r\n");
                        pos += 2;
                    } else {
                        pos++;
                    }
                }

                length = GetWindowTextLength(g_hCmdOutput);
                SendMessage(g_hCmdOutput, EM_SETSEL, length, length);
                SendMessage(g_hCmdOutput, EM_REPLACESEL, FALSE, (LPARAM)formattedOutput.c_str());
            }
        }

        // Wait for process to complete
        WaitForSingleObject(pi.hProcess, INFINITE);

        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
    } else {
        length = GetWindowTextLength(g_hCmdOutput);
        SendMessage(g_hCmdOutput, EM_SETSEL, length, length);
        SendMessage(g_hCmdOutput, EM_REPLACESEL, FALSE, (LPARAM)L"Error: Command failed to execute.\r\n");
    }

    CloseHandle(hReadPipe);

    // Add new prompt with current directory
    length = GetWindowTextLength(g_hCmdOutput);
    SendMessage(g_hCmdOutput, EM_SETSEL, length, length);
    prompt = GetFormattedPrompt();
    SendMessage(g_hCmdOutput, EM_REPLACESEL, FALSE, (LPARAM)(L"\r\n" + prompt).c_str());

    // Scroll to bottom and set focus back to input
    SendMessage(g_hCmdOutput, WM_VSCROLL, SB_BOTTOM, 0);
    SetFocus(g_hCmdEdit);
}

void UpdateStatusBox() {
    if (!g_hSideBar) return;

    // Get current time with better formatting
    SYSTEMTIME currentTime;
    GetLocalTime(&currentTime);
    wchar_t timeStr[256];
    GetTimeFormat(LOCALE_USER_DEFAULT, 0, &currentTime, L"HH:mm:ss", timeStr, 256);

    // Get date with better formatting
    wchar_t dateStr[256];
    GetDateFormat(LOCALE_USER_DEFAULT, 0, &currentTime, L"MM/dd/yyyy", dateStr, 256);

    // Get network status
    std::wstring networkStatus;
    GetNetworkStatus(networkStatus);

    // Get temperatures
    std::wstring cpuTemp, gpuTemp;
    GetSystemTemperatures(cpuTemp, gpuTemp);

    // Get uptime
    std::wstring uptime;
    GetUptime(uptime);

    // Get memory info
    MEMORYSTATUSEX memInfo = { sizeof(MEMORYSTATUSEX) };
    GlobalMemoryStatusEx(&memInfo);
    double memoryUsage = 100.0 - (memInfo.ullAvailPhys * 100.0 / memInfo.ullTotalPhys);

    // Get username
    wchar_t username[256];
    DWORD usernameLen = 256;
    GetUserName(username, &usernameLen);

    // Format status text with improved styling
    wchar_t statusText[2048];
    StringCchPrintf(statusText, 2048,
        L"   /\\__/\\   \r\n"
        L"  /`    '\\  \r\n"
        L" === 0  0 === \r\n"
        L"   \\  --  /  \r\n"
        L"  /        \\ \r\n"
        L" /          \\\r\n"
        L"|            |\r\n"
        L" \\  ||  ||  /\r\n"
        L"  \\_oo__oo_/ \r\n"
        L"========================\r\n"
        L"||  System Information  ||\r\n"
        L"========================\r\n"
        L"| User: %-14s|\r\n"
        L"| Time: %-14s|\r\n"
        L"| Date: %-14s|\r\n"
        L"========================\r\n"
        L"| Network: %-12s|\r\n"
        L"========================\r\n"
        L"| CPU Temp: %-10s|\r\n"
        L"| GPU Temp: %-10s|\r\n"
        L"| Memory: %.1f%% |\r\n"
        L"========================\r\n"
        L"| Uptime: %-12s|\r\n"
        L"========================\r\n"
        L"\r\n"
        L"   [F1L3 H4NDL3R]   \r\n"
        L"    Version 1.0.0    \r\n",
        username,
        timeStr, dateStr,
        networkStatus.c_str(),
        cpuTemp.c_str(), gpuTemp.c_str(),
        memoryUsage,
        uptime.c_str()
    );

    SetWindowText(g_hSideBar, statusText);
    InvalidateRect(g_hSideBar, NULL, TRUE);
}

void GetSystemTemperatures(std::wstring& cpuTemp, std::wstring& gpuTemp) {
    g_temperatureMonitor.getTemperatures(cpuTemp, gpuTemp);
}

void GetNetworkStatus(std::wstring& status) {
    DWORD flags = 0;
    if (InternetGetConnectedState(&flags, 0)) {
        if (flags & INTERNET_CONNECTION_LAN) status = L"LAN Connected";
        else if (flags & INTERNET_CONNECTION_MODEM) status = L"Modem Connected";
        else if (flags & INTERNET_CONNECTION_PROXY) status = L"Proxy Connected";
        else status = L"Connected";
    } else {
        status = L"Disconnected";
    }
}

void GetUptime(std::wstring& uptime) {
    SYSTEMTIME currentTime;
    GetLocalTime(&currentTime);

    FILETIME ftStart, ftCurrent;
    SystemTimeToFileTime(&g_startTime, &ftStart);
    SystemTimeToFileTime(&currentTime, &ftCurrent);

    ULARGE_INTEGER start, current;
    start.LowPart = ftStart.dwLowDateTime;
    start.HighPart = ftStart.dwHighDateTime;
    current.LowPart = ftCurrent.dwLowDateTime;
    current.HighPart = ftCurrent.dwHighDateTime;

    ULONGLONG diff = current.QuadPart - start.QuadPart;
    diff /= 10000000; // Convert to seconds

    int hours = (int)(diff / 3600);
    int minutes = (int)((diff % 3600) / 60);
    int seconds = (int)(diff % 60);

    wchar_t uptimeStr[64];
    StringCchPrintf(uptimeStr, 64, L"%02d:%02d:%02d", hours, minutes, seconds);
    uptime = uptimeStr;
}

// Add this helper function to get formatted prompt string
std::wstring GetFormattedPrompt() {
    wchar_t currentDir[MAX_PATH];
    GetCurrentDirectory(MAX_PATH, currentDir);
    std::wstring prompt(currentDir);
    prompt += L"> ";
    return prompt;
}

// Add ASCII art for sidebar
const wchar_t* SIDEBAR_ASCII_ART = 
L"   /\\__/\\   \r\n"
L"  /`    '\\  \r\n"
L" === 0  0 === \r\n"
L"   \\  --  /  \r\n"
L"  /        \\ \r\n"
L" /          \\\r\n"
L"|            |\r\n"
L" \\  ||  ||  /\r\n"
L"  \\_oo__oo_/ \r\n";

int main() {
	// This is just a placeholder to prevent compilation errors
	// The actual entry point is in the WinMain function
	return 0;
}