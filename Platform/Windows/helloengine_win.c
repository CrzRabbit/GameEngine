#include <windows.h>
#include <windowsx.h>
#include <tchar.h>

LRESULT CALLBACK WindowProc(HWND hWnd,
		UINT message,
		WPARAM wParam,
		LPARAM lParam);

int WINAPI WinMain(HINSTANCE hInstance,
		HINSTANCE hPrevInstance,
		LPTSTR lpCmdLine,
		int nCmdShow)
{
	// handle of the window
	HWND hWnd;

	// struct of window class information
	WNDCLASSEX wc;


	ZeroMemory(&wc, sizeof(WNDCLASSEX));
	
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = WindowProc;
	wc.hInstance = hInstance;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)COLOR_WINDOW;
	wc.lpszClassName = _T("WindowClass1");

	RegisterClassEx(&wc);

	hWnd = CreateWindowEx(0,
	                    _T("WindowClass1"), //name
	                    _T("Hello, engine!"), //title
	                    WS_OVERLAPPEDWINDOW, // style
	                    300, //x-position
	                    300, //y-position
	                    500, //width
	                    400, //height
	                    NULL, //parent window
	                    NULL, //menus
	                    hInstance, //application handle
	                    NULL); //mutiple windows

    ShowWindow(hWnd, nCmdShow);

    MSG msg;

    while(GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);

        DispatchMessage(&msg);
    }
}

LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch(message)
    {
        case WM_DESTROY:
        {
            PostQuitMessage(0);
            return 0;
        }
        break;
    }

    return DefWindowProc(hWnd, message, wParam, lParam);
}
