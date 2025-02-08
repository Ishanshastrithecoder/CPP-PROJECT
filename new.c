#include <windows.h>
#include <stdio.h>

#define BUTTON_ADD      1
#define BUTTON_SUBTRACT 2
#define BUTTON_MULTIPLY 
#define BUTTON_DIVIDE   4
#define BUTTON_EQUALS   5
#define BUTTON_CLEAR    6
#define BUTTON_HISTORY  7

LRESULT CALLBACK WindowProc(HWND, UINT, WPARAM, LPARAM);

HWND hInput1, hInput2, hResult, hListHistory;
char history[1024] = "";
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    const char CLASS_NAME[] = "SimpleCalculator";

    WNDCLASS wc = { };
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;

    RegisterClass(&wc);

    HWND hwnd = CreateWindowEx(0, CLASS_NAME, "Simple Calculator with History",
                               WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT,
                               800, 800, NULL, NULL, hInstance, NULL);

    if (hwnd == NULL) return 0;

    ShowWindow(hwnd, nCmdShow);

    MSG msg = { };
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}

void AddToHistory(const char *entry) {
    strcat(history, entry);
    strcat(history, "\r\n");
    SendMessage(hListHistory, LB_ADDSTRING, 0, (LPARAM)entry);
}

void CalculateAndDisplay(HWND hwnd, int operation) {
    char buffer1[256], buffer2[256], resultText[256];
    double num1, num2, result;

    GetWindowText(hInput1, buffer1, sizeof(buffer1));
    GetWindowText(hInput2, buffer2, sizeof(buffer2));

    num1 = atof(buffer1);
    num2 = atof(buffer2);

    switch (operation) {
        case BUTTON_ADD:      result = num1 + num2; sprintf(resultText, "%.2f + %.2f = %.2f", num1, num2, result); break;
        case BUTTON_SUBTRACT: result = num1 - num2; sprintf(resultText, "%.2f - %.2f = %.2f", num1, num2, result); break;
        case BUTTON_MULTIPLY: result = num1 * num2; sprintf(resultText, "%.2f * %.2f = %.2f", num1, num2, result); break;
        case BUTTON_DIVIDE:
            if (num2 == 0) {
                MessageBox(hwnd, "Cannot divide by zero", "Error", MB_OK | MB_ICONERROR);
                return;
            }
            result = num1 / num2;
            sprintf(resultText, "%.2f / %.2f = %.2f", num1, num2, result);
            break;
        default: return;
    }

    SetWindowText(hResult, resultText);
    AddToHistory(resultText);
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
        case WM_CREATE:
            hInput1 = CreateWindow("EDIT", "", WS_CHILD | WS_VISIBLE | WS_BORDER | ES_NUMBER,
                                   20, 20, 100, 25, hwnd, NULL, NULL, NULL);
            hInput2 = CreateWindow("EDIT", "", WS_CHILD | WS_VISIBLE | WS_BORDER | ES_NUMBER,
                                   150, 20, 100, 25, hwnd, NULL, NULL, NULL);
            hResult = CreateWindow("STATIC", "", WS_CHILD | WS_VISIBLE,
                                   20, 60, 230, 25, hwnd, NULL, NULL, NULL);

            CreateWindow("BUTTON", "+", WS_CHILD | WS_VISIBLE, 30, 100, 50, 25, hwnd, (HMENU) BUTTON_ADD, NULL, NULL);
            CreateWindow("BUTTON", "-", WS_CHILD | WS_VISIBLE, 90, 100, 50, 25, hwnd, (HMENU) BUTTON_SUBTRACT, NULL, NULL);
            CreateWindow("BUTTON", "*", WS_CHILD | WS_VISIBLE, 150, 100, 50, 25, hwnd, (HMENU) BUTTON_MULTIPLY, NULL, NULL);
            CreateWindow("BUTTON", "/", WS_CHILD | WS_VISIBLE, 210, 100, 50, 25, hwnd, (HMENU) BUTTON_DIVIDE, NULL, NULL);
            CreateWindow("BUTTON", "Clear", WS_CHILD | WS_VISIBLE, 200, 140, 100, 25, hwnd, (HMENU) BUTTON_CLEAR, NULL, NULL);

            CreateWindow("STATIC", "History:", WS_CHILD | WS_VISIBLE, 20, 180, 230, 20, hwnd, NULL, NULL, NULL);
            hListHistory = CreateWindow("LISTBOX", NULL, WS_CHILD | WS_VISIBLE | WS_BORDER | WS_VSCROLL,
                                        20, 210, 230, 100, hwnd, (HMENU) BUTTON_HISTORY, NULL, NULL);
            break;

        case WM_COMMAND:
            switch (LOWORD(wParam)) {
                case BUTTON_ADD: case BUTTON_SUBTRACT:
                case BUTTON_MULTIPLY: case BUTTON_DIVIDE:
                    CalculateAndDisplay(hwnd, LOWORD(wParam));
                    break;
                case BUTTON_CLEAR:
                    SetWindowText(hInput1, "");
                    SetWindowText(hInput2, "");
                    SetWindowText(hResult, "");
                    SendMessage(hListHistory, LB_RESETCONTENT, 0, 0);
                    strcpy(history, "");
                    break;
            }
            break;

        case WM_DESTROY:
            PostQuitMessage(0);
            break;

        default:
            return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
    return 0;
}