#include "framework.h"
#include "WinLab1.h"
#include <stdio.h>

#define MAX_LOADSTRING 100

HINSTANCE hInst;                               
WCHAR szTitle[MAX_LOADSTRING];                 
WCHAR szWindowClass[MAX_LOADSTRING];           

ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

static int sizeX = 100;
static int sizeY = 100;
static int sx, sy;
static COLORREF acrCustClr[8];
static DWORD rgbCurrent;

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_WINLAB1, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_WINLAB1));

    MSG msg;

    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int) msg.wParam;
}

ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_WINLAB1));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_WINLAB1);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance;

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}



BOOL __stdcall DlgProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_COMMAND:
    {
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            BOOL bSuccess;
            int sizeXtemp = GetDlgItemInt(hwnd, IDC_EDIT1, &bSuccess, FALSE);
            int sizeYtemp = GetDlgItemInt(hwnd, IDC_EDIT2, &bSuccess, FALSE);

            if (sizeXtemp > 0 && sizeYtemp > 0) {
                sizeX = sizeXtemp;
                sizeY = sizeYtemp;
                EndDialog(hwnd, 0);
                return 1;
            }
            else {
                EndDialog(hwnd, 0);
                return 1;
            }
        }
    }
    }
    return 0;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    
    switch (message)
    {
    case WM_LBUTTONDOWN: {
        POINT cursorPos;
        GetCursorPos(&cursorPos);
        ScreenToClient(hWnd, &cursorPos);

        char msg[50];
        sprintf(msg, "x: %i\ny : %i\n", cursorPos.x, cursorPos.y);
        const WCHAR* pwcsName;
        int size = MultiByteToWideChar(CP_ACP, 0, msg, -1, NULL, 0);
        pwcsName = new WCHAR[50];
        MultiByteToWideChar(CP_ACP, 0, msg, -1, (LPWSTR)pwcsName, size);
        MessageBox(0, pwcsName, L"Mouse Position", 0);
    }
        break;
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            switch (wmId)
            {
            case IDM_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;
            case IDM_COLOR:
                CHOOSECOLOR cc; 
                ZeroMemory(&cc, sizeof(cc));
                cc.lStructSize = sizeof(cc);
                cc.hwndOwner = hWnd;
                cc.lpCustColors = (LPDWORD)acrCustClr;
                cc.rgbResult = rgbCurrent;
                cc.Flags = CC_FULLOPEN | CC_RGBINIT;

                if (ChooseColor(&cc) == TRUE)
                {
                    rgbCurrent = cc.rgbResult;
                    InvalidateRect(hWnd, NULL, true);
                }
                break;
            case IDM_SIZES:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_DIALOG1), hWnd, DlgProc);
                InvalidateRect(hWnd, NULL, true);
                break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
    case WM_SIZE: {
        sx = LOWORD(lParam); //screen width
        sy = HIWORD(lParam); //screen height
    }
        break;
    case WM_CONTEXTMENU:
    {
        HMENU hMenu = CreatePopupMenu();

        AppendMenu(hMenu, MFT_STRING, IDM_COLOR, L"Выбор цвета");
        AppendMenu(hMenu, MFT_STRING, IDM_SIZES, L"Выбор шага сетки");

        TrackPopupMenu(hMenu, TPM_RIGHTBUTTON |
            TPM_TOPALIGN |
            TPM_LEFTALIGN,
            LOWORD(lParam),
            HIWORD(lParam), 0, hWnd, NULL);
        DestroyMenu(hMenu);
    }
    break;
    case WM_GETMINMAXINFO:
    {
        LPMINMAXINFO lpMMI = (LPMINMAXINFO)lParam;
        lpMMI->ptMinTrackSize.x = 100; //100 < standart min width
        lpMMI->ptMinTrackSize.y = 160; //160 = 60 for menu & 100 for workspace
    }
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            HGDIOBJ hOldPen = SelectObject(hdc, CreatePen(PS_SOLID, 1, rgbCurrent));
            int x = 0;
            int y = 0;
            while (x < sx) {
                MoveToEx(hdc, x, 0, NULL);
                LineTo(hdc, x, sy);
                x += sizeX;
            }
            while (y < sy) {
                MoveToEx(hdc, 0, y, NULL);
                LineTo(hdc, sx, y);
                y += sizeY;
            }
            DeleteObject(SelectObject(hdc, hOldPen));
            EndPaint(hWnd, &ps);
        }
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}


INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}
