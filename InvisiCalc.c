/* InvisiCalc - Inline Calculator

Acknowledgements:
Michael T. Smith / R.A.M. Technology. - For his system tray example code.
//test3
*/

/*
    Copyright 2009 by Kevin Pluck

    This file is part of InvisiCalc.

    InvisiCalc is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    InvisiCalc is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with InvisiCalc.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "InvisiCalc.h"
#include <windows.h>  // main Windows header
#include <shellapi.h> // Shell32 api
#include <stdio.h>
#include "Evaluate.h"
#define UWM_SYSTRAY (WM_USER + 1) // Sent to us by the systray

HWND ghwndNextWindowInClipboardChain;

LRESULT CALLBACK wndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK DialogProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam);
void CenterWindow (HWND hwnd);

HINSTANCE ghInst;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdLine, int nCmdShow)
{
    HWND hwnd;
    WNDCLASSEX wc;
    MSG msg;
    NOTIFYICONDATA nid;
    char *classname = "Noise42.NOTIFYICONDATA.hWnd";

    ghInst = hInstance;
    // Low priority -- don't soak up as many CPU cycles.
    // This won't make a difference unless we are doing background processing
    // in this process.
    SetPriorityClass(GetCurrentProcess(), IDLE_PRIORITY_CLASS);

    /* Create a window class for the window that receives systray notifications.
    The window will never be displayed, although there's no reason it couldn't
    be (it just wouldn't be very useful in this program, and in most others).
    */
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.style = 0;
    wc.lpfnWndProc = wndProc;
    wc.cbClsExtra = wc.cbWndExtra = 0;
    wc.hInstance = hInstance;
    wc.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_42));
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wc.lpszMenuName = NULL;
    wc.lpszClassName = classname;
    wc.hIconSm = (HICON)LoadImage(hInstance, MAKEINTRESOURCE(IDI_42), IMAGE_ICON, GetSystemMetrics(SM_CXSMICON), GetSystemMetrics(SM_CYSMICON), 0);
    RegisterClassEx(&wc);
    // Create window. Note that WS_VISIBLE is not used, and window is never shown
    hwnd = CreateWindowEx(0, classname, classname, WS_POPUP, CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);

    // Fill out NOTIFYICONDATA structure
    nid.cbSize = sizeof(NOTIFYICONDATA); // size
    nid.hWnd = hwnd; // window to receive notifications
    nid.uID = 1;     // application-defined ID for icon (can be any UINT value)
    nid.uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP;
    nid.uCallbackMessage = UWM_SYSTRAY; // message sent to nid.hWnd
    nid.hIcon = (HICON)LoadImage(hInstance, MAKEINTRESOURCE(IDI_42), IMAGE_ICON,
    GetSystemMetrics(SM_CXSMICON),
    GetSystemMetrics(SM_CYSMICON), 0); // 16x16 icon
    // szTip is the ToolTip text (64 byte array including NULL)
    strcpy(nid.szTip, INVISICALCVERSION);

    // NIM_ADD: Add icon; NIM_DELETE: Remove icon; NIM_MODIFY: modify icon
    Shell_NotifyIcon(NIM_ADD, &nid); // This adds the icon

    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return msg.wParam;
}


LRESULT CALLBACK wndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    POINT pt;
    HMENU hmenu, hpopup;
    NOTIFYICONDATA nid;
    LPTSTR    lptstrStringToProcess;
    char strStringToDisplay[200],strOldDisplayString[200];
    double fltEvaluation=0;

    int DisplayBubble(char *lptrstrBubbleTitle, char *lptrstrBubbleContent)
    {
        nid.cbSize = sizeof(NOTIFYICONDATA);
        nid.hWnd=hwnd;
        nid.uFlags = NIF_INFO;
        nid.dwInfoFlags = NIIF_INFO;
        nid.uTimeout = 0;
        nid.uID = 1;
        strcpy(nid.szInfoTitle,lptrstrBubbleTitle);
        strcpy(nid.szInfo, lptrstrBubbleContent);

        return Shell_NotifyIcon(NIM_MODIFY, &nid);
    }

    LPTSTR GetClipboardContent()
    {
        LPTSTR lptstrToReturn = NULL,lptstrClipboardContents;
        HGLOBAL   hglb; //global handle for clipboard

        //Is it text?
        if (!IsClipboardFormatAvailable(CF_TEXT)) goto stop;

        if (!OpenClipboard(hwnd)) goto stop;

        hglb = GetClipboardData(CF_TEXT);

        if(hglb==NULL) goto closeandstop;

        lptstrClipboardContents=GlobalLock(hglb);

        if(lptstrClipboardContents!=NULL)
        {
            if((lptstrToReturn = malloc(strlen(lptstrClipboardContents)+1))==NULL) goto closeandstop;

            strcpy(lptstrToReturn,lptstrClipboardContents);
        }

        GlobalUnlock(hglb);

        closeandstop:
            CloseClipboard();

        stop:
            // Pass the message to the next window in clipboard viewer chain.
            SendMessage(ghwndNextWindowInClipboardChain, message, wParam, lParam);

            return lptstrToReturn;
    }

    switch (message)
    {
        case WM_CREATE:
            ghwndNextWindowInClipboardChain=SetClipboardViewer(hwnd);
        return TRUE;


        case WM_CHANGECBCHAIN:

            // If the next window is closing, repair the chain.
            if ((HWND) wParam == ghwndNextWindowInClipboardChain)
            ghwndNextWindowInClipboardChain = (HWND) lParam;

            // Otherwise, pass the message to the next link.
            else if (ghwndNextWindowInClipboardChain != NULL)
            SendMessage(ghwndNextWindowInClipboardChain, message, wParam, lParam);

        break;


        case WM_DRAWCLIPBOARD:  // clipboard contents changed.

            lptstrStringToProcess=GetClipboardContent();

            if(lptstrStringToProcess!=NULL)
            {

                /*
                Evaluate
                    Returns:
                         1 - If strInfixCode can be evaluated
                         0 - If strInfixCode cannot be evaluated
                        -1 - If strInfixCode can be evaluated but a divide by zero occured
                        -2 - If strInfixCode can be evaluated but an invalid input occured E.g.: sqrt(-1)
                */
                switch(Evaluate(lptstrStringToProcess,&fltEvaluation))
                {
                    case 1:
                        sprintf(strStringToDisplay,"%s = %.10g.\nCtrl-c to copy result.",lptstrStringToProcess,fltEvaluation);

                        if(strcmp(strOldDisplayString,strStringToDisplay)!=0)
                            DisplayBubble("InvisiCalc",strStringToDisplay);
                        else
                        {
                            //ctrl-c has been pressed a second time
                            char strEvaluation[50];
                            sprintf(strEvaluation,"%.10g",fltEvaluation);

                            const size_t len = strlen(strEvaluation) + 1;

                            HGLOBAL hMem =  GlobalAlloc(GMEM_MOVEABLE, len);
                            memcpy(GlobalLock(hMem), strEvaluation, len);
                            GlobalUnlock(hMem);

                            OpenClipboard(0);
                                EmptyClipboard();
                                SetClipboardData(CF_TEXT, hMem);
                            CloseClipboard();

                            DisplayBubble("InvisiCalc","Copied");
                        }

                        strcpy(strOldDisplayString,strStringToDisplay);
                    break;

                    case -1:
                        DisplayBubble("InvisiCalc","Divide by zero occured.");
                    break;

                    case -2:
                        DisplayBubble("InvisiCalc","Invalid input.");
                    break;
                }
            }

            free(lptstrStringToProcess);
        break;

        case WM_DESTROY:
            nid.cbSize = sizeof(NOTIFYICONDATA);
            nid.hWnd = hwnd;
            nid.uID = 1;
            nid.uFlags = NIF_TIP; // not really sure what to put here, but it works
            Shell_NotifyIcon(NIM_DELETE, &nid); // This removes the icon
            ChangeClipboardChain(hwnd, ghwndNextWindowInClipboardChain);
            PostQuitMessage(0);
        return TRUE;

        case UWM_SYSTRAY: // We are being notified of mouse activity over the icon
        switch (lParam)
        {
            case WM_RBUTTONUP: // Let's track a popup menu
                GetCursorPos(&pt);
                hmenu = LoadMenu(ghInst, MAKEINTRESOURCE(IDM_CONTEXTMAIN));
                hpopup = GetSubMenu(hmenu, 0);

                SetForegroundWindow(hwnd);
                /* We specifiy TPM_RETURNCMD, so TrackPopupMenu returns the menu
                selection instead of returning immediately and our getting a
                WM_COMMAND with the selection. You don't have to do it this way.
                */
                switch (TrackPopupMenu(hpopup,  // Popup menu to track
                        TPM_RETURNCMD |         // Return menu code
                        TPM_RIGHTBUTTON,        // Track right mouse button?
                        pt.x, pt.y,             // screen coordinates
                        0,                      // reserved
                        hwnd,                   // owner
                        NULL))                  // LPRECT user can click in without dismissing menu
                {
                    case IDM_EXIT:
                        DestroyWindow(hwnd);
                    break;

                    case IDM_ABOUT:
                        DialogBox(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_ABOUT), hwnd, DialogProc);
                    break;

                    case IDM_HELP:
                        DialogBox(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_HELP), hwnd, DialogProc);
                    break;
                }
                PostMessage(hwnd, 0, 0, 0); // see above
                DestroyMenu(hmenu); // Delete loaded menu and reclaim its resources
            break;
        }
        return TRUE; // I don't think that it matters what you return.
    }
    return DefWindowProc(hwnd, message, wParam, lParam);
}

//used for both About and Help windows
BOOL CALLBACK DialogProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam)
{
    switch(Message)
    {
        case WM_INITDIALOG:
            CenterWindow(hwnd);
        break;

        case WM_COMMAND:
            switch(LOWORD(wParam))
            {
                case IDOK:
                    EndDialog(hwnd, IDOK);
                break;

                case IDC_LINK:
                    ShellExecute(hwnd,"open", APPLINK, NULL, NULL, SW_SHOWNORMAL);
                    EndDialog(hwnd,0);
                break;
            }
        break;

        case WM_CLOSE:
            EndDialog(hwnd, 0);
        break;

        default:
            return FALSE;
    }
    return TRUE;
}


void CenterWindow (HWND hwnd)
{
    RECT rc;

    GetWindowRect(hwnd,&rc);

    int nWidth = rc.right - rc.left;
    int nHeight = rc.bottom - rc.top;

    int X = ((int) GetSystemMetrics(SM_CXFULLSCREEN) - nWidth) /2;       // center window horizontally
    int Y = ((int) GetSystemMetrics(SM_CYFULLSCREEN) - nHeight) /2;      // and vertically

    MoveWindow(hwnd,X,Y,nWidth,nHeight,TRUE);
}

