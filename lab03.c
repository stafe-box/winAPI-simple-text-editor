#include <windows.h>
#define ID_BEEP 1000
#define ID_QUIT 1001
#define ID_TEXT 1002
#define ID_EDIT 1003
#define ID_SAVE 1004
#define ID_COPY 1009
#define ID_PASTE 1008
#define ID_LOAD 1007

const char g_szClassName[] = "myWindowClass";
HWND edit;
char buff[1024];

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
        case WM_CREATE:
            edit = CreateWindowW(L"Edit", L"You can write text here",
                                 WS_CHILD | WS_VISIBLE | WS_VSCROLL | ES_MULTILINE | ES_AUTOVSCROLL,
                                 15, 15, 300, 200, hwnd, (HMENU)ID_EDIT, NULL, NULL);
            CreateWindowW(L"Button", L"Открыть", WS_VISIBLE | WS_CHILD, 20, 250, 80, 25, hwnd,
                          (HMENU)ID_LOAD, NULL, NULL);
            CreateWindowW(L"Button", L"Справка", WS_VISIBLE | WS_CHILD, 120, 250, 80, 25, hwnd,
                          (HMENU)ID_TEXT, NULL, NULL);
             CreateWindowW(L"Button", L"Сохранить", WS_VISIBLE | WS_CHILD, 220, 215, 80, 25, hwnd,
                          (HMENU)ID_SAVE, NULL, NULL);
            CreateWindowW(L"Button", L"Выйти", WS_VISIBLE | WS_CHILD, 220, 250, 80, 25, hwnd,
                          (HMENU)ID_QUIT, NULL, NULL);
            CreateWindowW(L"Button", L"Копировать", WS_VISIBLE | WS_CHILD, 20, 215, 80, 25, hwnd,
                          (HMENU)ID_COPY, NULL, NULL);
            CreateWindowW(L"Button", L"Вставить", WS_VISIBLE | WS_CHILD, 120, 215, 80, 25, hwnd,
                          (HMENU)ID_PASTE, NULL, NULL);
            break;
        case WM_COMMAND:
    switch (LOWORD(wParam))
    {
        case ID_TEXT:;
            MessageBox(NULL, "Simple text editor", "help!", MB_ICONINFORMATION | MB_OK);
            break;
        case ID_COPY:
            // get text from the edit control
            //char buff[1024];
            GetWindowText(edit, buff, 1024);
            // open clipboard and empty it
            if (!OpenClipboard(hwnd))
            {
                MessageBox(NULL, "Unable to open clipboard.", "Error", MB_ICONERROR | MB_OK);
            }
            else
            {
                EmptyClipboard();
                // allocate memory for the text and copy it to the clipboard
                HGLOBAL hClipboard = GlobalAlloc(GMEM_MOVEABLE, strlen(buff) + 1);
                char *pClipboard = (char *)GlobalLock(hClipboard);
                strcpy(pClipboard, buff);
                GlobalUnlock(hClipboard);
                SetClipboardData(CF_TEXT, hClipboard);
                CloseClipboard();
            }
            break;
        case ID_PASTE:
            // open clipboard and get the text
            if (!OpenClipboard(hwnd))
            {
                MessageBox(NULL, "Unable to open clipboard.", "Error", MB_ICONERROR | MB_OK);
            }
            else
            {
                HANDLE hClipboard = GetClipboardData(CF_TEXT);
                if (hClipboard != NULL)
                {
                    //char buff[1024];
                    GetWindowText(edit, buff, 1024);
                    char *pClipboard = (char *)GlobalLock(hClipboard);
                    // set the text to the edit control
                    strcat(buff, pClipboard);
                    SetWindowText(edit, buff);
                    GlobalUnlock(hClipboard);
                }
                CloseClipboard();
            }
            break;
        case ID_QUIT:
            PostQuitMessage(0);
            break;
        case ID_SAVE:
            // Get text from the edit control
            //char buff[1024];
            GetWindowText(edit, buff, 1024);

            // Open the file dialog to get the file name
            OPENFILENAME ofn;
            ZeroMemory(&ofn, sizeof(ofn));
            ofn.lStructSize = sizeof(ofn);
            ofn.hwndOwner = hwnd;
            ofn.lpstrFilter = "Text Files (*.txt)\0*.txt\0All Files (*.*)\0*.*\0";
            char filename[MAX_PATH] = { 0 };
            ofn.lpstrFile = filename;
            ofn.nMaxFile = MAX_PATH;
            ofn.Flags = OFN_EXPLORER | OFN_PATHMUSTEXIST | OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT;
            ofn.lpstrDefExt = "txt";
            if (GetSaveFileName(&ofn))
            {
                // Open the file and write the text to it
                HANDLE hFile = CreateFile(filename, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
                if (hFile != INVALID_HANDLE_VALUE)
                {
                    DWORD dwBytesWritten;
                    WriteFile(hFile, buff, strlen(buff), &dwBytesWritten, NULL);
                    CloseHandle(hFile);
                }
                else
                {
                    MessageBox(NULL, "Unable to open file for writing.", "Error", MB_ICONERROR | MB_OK);
                }
            }
            break;
        case ID_LOAD:
            // open the file dialog to get the file name
            ZeroMemory(&ofn, sizeof(ofn));
            ofn.lStructSize = sizeof(ofn);
            ofn.hwndOwner = hwnd;
            ofn.lpstrFilter = "Text Files (*.txt)\0*.txt\0All Files (*.*)\0*.*\0";
            ofn.lpstrFile = buff;
            ofn.nMaxFile = 1024;
            ofn.Flags = OFN_EXPLORER | OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
            if (GetOpenFileName(&ofn))
            {
                // open the file and read the text from it
                HANDLE hFile = CreateFile(ofn.lpstrFile, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
                if (hFile != INVALID_HANDLE_VALUE)
                {
                    DWORD dwFileSize = GetFileSize(hFile, NULL);
                    if (dwFileSize > 0)
                    {
                        char *pFileContents = (char *)GlobalAlloc(GPTR, dwFileSize + 1);
                        DWORD dwBytesRead;
                        if (ReadFile(hFile, pFileContents, dwFileSize, &dwBytesRead, NULL))
                        {
                            pFileContents[dwBytesRead] = 0;
                            // set the text to the edit control
                            SetWindowText(edit, pFileContents);
                        }
                        GlobalFree(pFileContents);
                    }
                    CloseHandle(hFile);
                }
                else
                {
                    MessageBox(NULL, "Unable to open file for reading.", "Error", MB_ICONERROR | MB_OK);
                }
            }
            break;
    }
    break;
        case WM_CLOSE:
            DestroyWindow(hwnd);
            break;
        case WM_DESTROY:
            PostQuitMessage(0);
            break;
        default:
            return DefWindowProc(hwnd, msg, wParam, lParam);
    }
    return 0;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    WNDCLASSEX wc;
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.style = 0;
    wc.lpfnWndProc = WndProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = hInstance;
    wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wc.lpszMenuName = NULL;
    wc.lpszClassName = g_szClassName;
    wc.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

    if (!RegisterClassEx(&wc))
    {
        MessageBox(NULL, "Window Registration Failed!", "Error!", MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }
    HWND hwnd;
    hwnd =
        CreateWindowEx(WS_EX_CLIENTEDGE, g_szClassName, "Hello from WinAPI!", WS_OVERLAPPEDWINDOW,
                       CW_USEDEFAULT, CW_USEDEFAULT, 350, 330, NULL, NULL, hInstance, NULL);
    if (hwnd == NULL)
    {
        MessageBox(NULL, "Window Creation Failed!", "Error!", MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }
    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);

    MSG Msg;
    while (GetMessage(&Msg, NULL, 0, 0) > 0)
    {
        TranslateMessage(&Msg);
        DispatchMessage(&Msg);
    }
    return Msg.wParam;
}
